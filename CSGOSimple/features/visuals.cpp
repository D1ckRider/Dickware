
#include <algorithm>

#include "visuals.hpp"

//#include "../options.hpp"
#include "../Settings.h"
#include "../ConfigSystem.h"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../Resolver.h"
#include "../RuntimeSaver.h"
#include "../Logger.h"
#include "../ConsoleHelper.h"
#include "../Autowall.h"
#include "../Rbot.h"

RECT GetBBox ( C_BaseEntity* ent )
{
    RECT rect{};
    auto collideable = ent->GetCollideable();

    if ( !collideable )
        return rect;

    auto min = collideable->OBBMins();
    auto max = collideable->OBBMaxs();

    const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

    Vector points[] =
    {
        Vector ( min.x, min.y, min.z ),
        Vector ( min.x, max.y, min.z ),
        Vector ( max.x, max.y, min.z ),
        Vector ( max.x, min.y, min.z ),
        Vector ( max.x, max.y, max.z ),
        Vector ( min.x, max.y, max.z ),
        Vector ( min.x, min.y, max.z ),
        Vector ( max.x, min.y, max.z )
    };

    Vector pointsTransformed[8];

    for ( int i = 0; i < 8; i++ )
        Math::VectorTransform ( points[i], trans, pointsTransformed[i] );

    Vector screen_points[8] = {};

    for ( int i = 0; i < 8; i++ )
    {
        if ( !Math::WorldToScreen ( pointsTransformed[i], screen_points[i] ) )
            return rect;
    }

    auto left = screen_points[0].x;
    auto top = screen_points[0].y;
    auto right = screen_points[0].x;
    auto bottom = screen_points[0].y;

    for ( int i = 1; i < 8; i++ )
    {
        if ( left > screen_points[i].x )
            left = screen_points[i].x;

        if ( top < screen_points[i].y )
            top = screen_points[i].y;

        if ( right < screen_points[i].x )
            right = screen_points[i].x;

        if ( bottom > screen_points[i].y )
            bottom = screen_points[i].y;
    }

    return RECT{ ( long ) left, ( long ) top, ( long ) right, ( long ) bottom };
}

Visuals::Visuals()
{
    InitializeCriticalSection ( &cs );
}

Visuals::~Visuals()
{
    DeleteCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------
void Visuals::Render()
{
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin ( C_BasePlayer* pl )
{
    if ( pl->IsDormant() || !pl->IsAlive() )
        return false;

    ctx.pl = pl;
    ctx.is_enemy = pl->IsEnemy();
    ctx.is_visible = g_LocalPlayer->CanSeePlayer ( pl, HITBOX_CHEST );

    //if (!ctx.is_enemy && g_Config.GetBool("esp_enemies_only")) //to fix
    //	return false;

    ctx.clr = ctx.is_enemy ? ( ctx.is_visible ? g_Config.GetColor ( "color_esp_enemy_visible" ) : g_Config.GetColor ( "color_esp_enemy_occluded" ) ) : ( ctx.is_visible ? g_Config.GetColor ( "color_esp_ally_visible" ) : g_Config.GetColor ( "color_esp_ally_occluded" ) );

    auto head = pl->GetHitboxPos ( HITBOX_HEAD );
    auto origin = pl->m_vecOrigin();

    head.z += 15;

    if ( !Math::WorldToScreen ( head, ctx.head_pos ) ||
            !Math::WorldToScreen ( origin, ctx.feet_pos ) )
        return false;

    auto h = fabs ( ctx.head_pos.y - ctx.feet_pos.y );
    auto w = h / 1.65f;

    ctx.bbox.left = static_cast<long> ( ctx.feet_pos.x - w * 0.5f );
    ctx.bbox.right = static_cast<long> ( ctx.bbox.left + w );
    ctx.bbox.bottom = static_cast<long> ( ctx.feet_pos.y );
    ctx.bbox.top = static_cast<long> ( ctx.head_pos.y );

    if ( ctx.bbox.left > ctx.bbox.right )
    {
        ctx.bbox.left = ctx.bbox.right;
        ctx.ShouldDrawBox = false;
    }

    if ( ctx.bbox.bottom < ctx.bbox.top )
    {
        ctx.bbox.bottom = ctx.bbox.top;
        ctx.ShouldDrawBox = false;
    }

    return true;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox()
{
    if ( !g_LocalPlayer || !ctx.ShouldDrawBox )
        return;

    int mode = ctx.boxmode;

    if ( !g_LocalPlayer->IsAlive() )
        ctx.boxmode = 0;

    switch ( ctx.boxmode )
    {
        case 0:
            //Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1.f);
            VGSHelper::Get().DrawBox ( ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.BoxClr, 1.f );
            break;

        case 1:
            float edge_size = 25.f;

            if ( ctx.pl != g_LocalPlayer )
                edge_size = 4000.f / Math::VectorDistance ( g_LocalPlayer->m_vecOrigin(), ctx.pl->m_vecOrigin() );

            VGSHelper::Get().DrawBoxEdges ( ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.BoxClr, edge_size, 1.f );

            break;
    }
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderName()
{
    player_info_t info = ctx.pl->GetPlayerInfo();

    auto sz = g_pDefaultFont->CalcTextSizeA ( 12.f, FLT_MAX, 0.0f, info.szName );

    //Render::Get().RenderText(info.szName, ctx.bbox.right + 4.f, ctx.head_pos.y - sz.y + TextHeight, 14.f, ctx.clr);
    VGSHelper::Get().DrawText ( info.szName, ( ctx.bbox.left + ( ( ctx.bbox.right - ctx.bbox.left ) / 2 ) ) - ( sz.x / 2 ), ctx.head_pos.y - sz.y - 4.f, ctx.NameClr, 12.f );
    //TextHeight += 14.f;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{
    if ( !ctx.ShouldDrawBox )
        return;

    auto  hp = ctx.pl->m_iHealth();

    if ( hp > 100 )
        hp = 100;

    int green = int ( hp * 2.55f );
    int red = 255 - green;

    RenderLine ( ctx.healthpos, Color ( red, green, 0, 255 ), ( hp ) / 100.f );
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
    if ( !ctx.ShouldDrawBox )
        return;

    auto armour = ctx.pl->m_ArmorValue();
    RenderLine ( ctx.armourpos, ctx.ArmourClr, ( armour ) / 100.f );
}
void Visuals::Player::RenderLbyUpdateBar()
{
    if ( !ctx.ShouldDrawBox )
        return;

    int i = ctx.pl->EntIndex();

    if ( ctx.pl->m_vecVelocity().Length2D() > 0.1f || ! ( ctx.pl->m_fFlags() & FL_ONGROUND ) )
        return;

    if ( !g_Resolver.GResolverData[i].CanuseLbyPrediction )
        return;

    float percent = 1.f - ( ( g_Resolver.GResolverData[i].NextPredictedLbyBreak - ctx.pl->m_flSimulationTime() ) / 1.1f );

    if ( percent < 0.f || percent > 1.f )
        return;

    RenderLine ( ctx.lbyupdatepos, ctx.LbyTimerClr, percent );
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeaponName()
{
    auto weapon = ctx.pl->m_hActiveWeapon().Get();

    if ( !weapon )
        return;

    auto text = weapon->GetCSWeaponData()->szWeaponName + 7;
    auto sz = g_pDefaultFont->CalcTextSizeA ( 12, FLT_MAX, 0.0f, text );
    //Render::Get().RenderText(text, ctx.feet_pos.x, ctx.feet_pos.y, 14.f, ctx.clr, true);
    //Render::Get().RenderText(text, ctx.bbox.right + 4.f, ctx.head_pos.y - sz.y + TextHeight, 14.f, ctx.clr);
    //VGSHelper::Get().DrawText ( text, ctx.bbox.right + 2.f + ctx.PosHelper.right, ctx.head_pos.y - sz.y + TextHeight, ctx.WeaponClr, 12 );
    VGSHelper::Get().DrawText ( text, ctx.bbox.right + 2.f + ctx.PosHelper.right, ctx.head_pos.y - sz.y + TextHeight, ctx.WeaponClr, 12 );
    TextHeight += 12.f;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{

    int screen_w, screen_h;
    g_EngineClient->GetScreenSize ( screen_w, screen_h );

    //Render::Get().RenderLine(screen_w / 2.f, (float)screen_h,
    //	ctx.feet_pos.x, ctx.feet_pos.y, ctx.clr);

    VGSHelper::Get().DrawLine ( screen_w / 2.f, ( float ) screen_h, ctx.feet_pos.x, ctx.feet_pos.y, ctx.SnaplineClr );
}
void Visuals::Player::DrawPlayerDebugInfo()
{
    if ( !g_LocalPlayer || ctx.pl == g_LocalPlayer )
        return;

    if ( !ctx.pl->IsEnemy() )
        return;

    std::string t1 = "missed shots: " + std::to_string ( g_Resolver.GResolverData[ctx.pl->EntIndex()].Shots );
    std::string t2 = "mode: ";// +std::to_string(g_Resolver.GResolverData[ctx.pl->EntIndex()].Shots);
    std::string t3 = "detected: ";
    std::string t4 = g_Resolver.GResolverData[ctx.pl->EntIndex()].Fake ? "fake" : "real";
    std::string t5 = "velocity: " + std::to_string ( ctx.pl->m_vecVelocity().Length2D() );
    int i = ctx.pl->EntIndex();

    switch ( g_Resolver.GResolverData[i].mode )
    {
        case ResolverModes::NONE:
            t2 += "none";
            break;

        case ResolverModes::FREESTANDING:
            t2 += "FREESTANDING";
            break;

        case ResolverModes::EDGE:
            t2 += "EDGE";
            break;

        case ResolverModes::MOVE_STAND_DELTA:
            t2 += "MOVE_STAND_DELTA";
            break;

        case ResolverModes::FORCE_LAST_MOVING_LBY:
            t2 += "FORCE_LAST_MOVING_LBY";
            break;

        case ResolverModes::FORCE_FREESTANDING:
            t2 += "FORCE_FREESTANDING";
            break;

        case ResolverModes::BRUTFORCE_ALL_DISABLED:
            t2 += "BRUTFORCE_ALL_DISABLED";
            break;

        case ResolverModes::BRUTFORCE:
            t2 += "BRUTFORCE";
            break;

        case ResolverModes::FORCE_MOVE_STAND_DELTA:
            t2 += "FORCE_MOVE_STAND_DELTA";
            break;

        case ResolverModes::FORCE_LBY:
            t2 += "FORCE_LBY";
            break;

        case ResolverModes::MOVING:
            t2 += "MOVING";
            break;

        case ResolverModes::LBY_BREAK:
            t2 += "LBY_BREAK";
            break;

        case ResolverModes::SPINBOT:
            t2 += "SPINBOT";
            break;

        case ResolverModes::AIR_FREESTANDING:
            t2 += "AIR_FREESTANDING";
            break;

        case ResolverModes::AIR_BRUTFORCE:
            t2 += "AIR_BRUTFORCE";
            break;

        case ResolverModes::FAKEWALK_FREESTANDING:
            t2 += "FAKEWALK_FREESTANDING";
            break;

        case ResolverModes::FAKEWALK_BRUTFORCE:
            t2 += "FAKEWALK_BRUTFORCE";
            break;

        case ResolverModes::BACKWARDS:
            t2 += "BACKWARDS";
            break;

        case ResolverModes::FORCE_BACKWARDS:
            t2 += "FORCE_BACKWARDS";
            break;
    }

    switch ( g_Resolver.GResolverData[i].detection )
    {
        case ResolverDetections::FAKEWALKING:
            t3 += "Fakewalking";
            break;

        case ResolverDetections::AIR:
            t3 += "Air";
            break;

        case ResolverDetections::MOVING:
            t3 += "Moving";
            break;

        case ResolverDetections::STANDING:
            t3 += "Standing";
            break;
    }

    VGSHelper::Get().DrawText ( t1, ctx.bbox.right + 48.f, ctx.head_pos.y, Color::White );
    VGSHelper::Get().DrawText ( t2, ctx.bbox.right + 48.f, ctx.head_pos.y + 14.f, Color::White );
    VGSHelper::Get().DrawText ( t3, ctx.bbox.right + 48.f, ctx.head_pos.y + 28.f, Color::White );
    VGSHelper::Get().DrawText ( t4, ctx.bbox.right + 48.f, ctx.head_pos.y + 42.f, Color::White );
    VGSHelper::Get().DrawText ( t5, ctx.bbox.right + 48.f, ctx.head_pos.y + 56.f, Color::White );
}
void Visuals::Player::RenderLine ( DrawSideModes mode, Color color, float percent )
{
    float box_h = ( float ) fabs ( ctx.bbox.bottom - ctx.bbox.top );
    float box_w = ( float ) fabs ( ctx.bbox.right - ctx.bbox.left );
    float off = 4;
    float x = 0;
    float y = 0;
    float x2 = 0;
    float y2 = 0;

    //float x3 = 0;
    //float y3 = 0;
    switch ( mode )
    {
        case DrawSideModes::TOP:
            off = ctx.PosHelper.top;
            x = ctx.bbox.left;
            y = ctx.bbox.top + off;
            x2 = x + ( box_w * percent );
            y2 = y;
            ctx.PosHelper.top += 8;
            break;

        case DrawSideModes::RIGHT:
            off = ctx.PosHelper.right;
            x = ctx.bbox.right + off;
            y = ctx.bbox.top;
            x2 = x + 4;
            y2 = y + ( box_h * percent );
            ctx.PosHelper.right += 8;
            break;

        case DrawSideModes::BOTTOM:
            off = ctx.PosHelper.bottom;
            x = ctx.bbox.left;
            y = ctx.bbox.bottom + off;
            x2 = x + ( box_w * percent );
            y2 = y;
            ctx.PosHelper.bottom += 8;
            break;

        case DrawSideModes::LEFT:
            off = ctx.PosHelper.left;
            x = ctx.bbox.left - ( off * 2 );
            y = ctx.bbox.top;
            x2 = x + 4;
            y2 = y + ( box_h * percent );
            ctx.PosHelper.left += 8;
            break;
    }

    //Render::Get().RenderBox(x, y, x + w, y + h, Color::Black, 1.f, true);
    //Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(0, 50, 255, 255), 1.f, true);
    if ( mode == DrawSideModes::LEFT || mode == DrawSideModes::RIGHT )
        VGSHelper::Get().DrawFilledBox ( x, y, x2, y + box_h, Color ( 0, 0, 0, 100 ) );
    else
        VGSHelper::Get().DrawFilledBox ( x, y, x + box_w, y2, Color ( 0, 0, 0, 100 ) );

    VGSHelper::Get().DrawFilledBox ( x + 1, y + 1, x2 - 1, y2 - 2, color );
}
void Visuals::Player::RenderResolverInfo()
{
    if ( g_Resolver.GResolverData[ctx.pl->EntIndex()].Fake )
    {
        char* t1 = "Fake";
        auto sz = g_pDefaultFont->CalcTextSizeA ( 12, FLT_MAX, 0.0f, t1 );
        //VGSHelper::Get().DrawText ( t1, ctx.bbox.right + 8.f, ctx.head_pos.y - sz.y + TextHeight, ctx.InfoClr, 12 );
        VGSHelper::Get().DrawText ( t1, ctx.bbox.right + 2.f + ctx.PosHelper.right, ctx.head_pos.y - sz.y + TextHeight, ctx.WeaponClr, 12 );
        TextHeight += 12.f;
    }

    if ( g_Resolver.GResolverData[ctx.pl->EntIndex()].BreakingLC )
    {
        char* t1 = "LC";
        auto sz = g_pDefaultFont->CalcTextSizeA ( 12, FLT_MAX, 0.0f, t1 );
        //VGSHelper::Get().DrawText ( t1, ctx.bbox.right + 8.f, ctx.head_pos.y - sz.y + TextHeight, ctx.InfoClr, 12 );
        VGSHelper::Get().DrawText ( t1, ctx.bbox.right + 2.f + ctx.PosHelper.right, ctx.head_pos.y - sz.y + TextHeight, ctx.WeaponClr, 12 );
        TextHeight += 12.f;
    }
}
//--------------------------------------------------------------------------------
void Visuals::RenderCrosshair()
{
    int w, h;

    g_EngineClient->GetScreenSize ( w, h );

    int cx = w / 2;
    int cy = h / 2;
    Color clr = g_Config.GetColor ( "color_esp_crosshair" );
    VGSHelper::Get().DrawLine ( cx - 5, cy, cx + 5, cy, clr );
    VGSHelper::Get().DrawLine ( cx, cy - 5, cx, cy + 5, clr );
}
//--------------------------------------------------------------------------------
void Visuals::RenderWeapon ( C_BaseCombatWeapon* ent )
{
    auto clean_item_name = [] ( const char* name ) -> const char*
    {
        if ( name[0] == 'C' )
            name++;

        auto start = strstr ( name, "Weapon" );

        if ( start != nullptr )
            name = start + 6;

        return name;
    };

    // We don't want to Render weapons that are being held
    if ( ent->m_hOwnerEntity().IsValid() )
        return;

    auto bbox = GetBBox ( ent );

    if ( bbox.right == 0 || bbox.bottom == 0 )
        return;

    Color clr = Color::White;//g_Config.GetColor("color_esp_weapons");

    //Render::Get().RenderBox(bbox, clr);

    VGSHelper::Get().DrawBox ( bbox.left, bbox.top, bbox.right, bbox.bottom, clr );

    auto name = clean_item_name ( ent->GetClientClass()->m_pNetworkName );

    auto sz = g_pDefaultFont->CalcTextSizeA ( 12.f, FLT_MAX, 0.0f, name );
    int w = bbox.right - bbox.left;


    VGSHelper::Get().DrawText ( name, ( bbox.left + w * 0.5f ) - sz.x * 0.5f, bbox.bottom + 1, clr, 12 );
    //Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 12.f, clr);
}
//--------------------------------------------------------------------------------
void Visuals::RenderDefuseKit ( C_BaseEntity* ent )
{
    if ( ent->m_hOwnerEntity().IsValid() )
        return;

    auto bbox = GetBBox ( ent );

    if ( bbox.right == 0 || bbox.bottom == 0 )
        return;

    Color clr = g_Config.GetColor ( "color_esp_defuse" );
    //Render::Get().RenderBox(bbox, clr);
    VGSHelper::Get().DrawBox ( bbox.left, bbox.top, bbox.right, bbox.bottom, clr );

    auto name = "Defuse Kit";
    auto sz = g_pDefaultFont->CalcTextSizeA ( 14.f, FLT_MAX, 0.0f, name );
    int w = bbox.right - bbox.left;
    //Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, clr);
    VGSHelper::Get().DrawText ( name, ( bbox.left + w * 0.5f ) - sz.x * 0.5f, bbox.bottom + 1, clr, 12 );
}
//--------------------------------------------------------------------------------
void Visuals::RenderPlantedC4 ( C_BaseEntity* ent )
{
    auto bbox = GetBBox ( ent );

    if ( bbox.right == 0 || bbox.bottom == 0 )
        return;

	Color clr = Settings::Visual::GlobalESP.BombColor;
    int bombTimer = std::ceil ( ent->m_flC4Blow() - g_GlobalVars->curtime );

    if ( bombTimer < 0.f )
        return;

    //Render::Get().RenderBox(bbox, clr);
    VGSHelper::Get().DrawBox ( bbox.left, bbox.top, bbox.right, bbox.bottom, clr );

    std::string timer = std::to_string ( bombTimer );

    auto sz = g_pDefaultFont->CalcTextSizeA ( 12.f, FLT_MAX, 0.0f, timer.c_str() );
    int w = bbox.right - bbox.left;

    VGSHelper::Get().DrawText ( timer, ( bbox.left + w * 0.5f ) - sz.x * 0.5f, bbox.bottom + 1, clr, 12 );
}

void Visuals::RenderSoundESP()
{
	if (!g_Saver.StepInfo.empty())
	{
		for (size_t i = 0; i < g_Saver.StepInfo.size(); i++)
		{
			if (g_GlobalVars->realtime - g_Saver.StepInfo[i].SoundTime < 1.f)
			{
				Vector pos;
				auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, "Step");
				Math::WorldToScreen(g_Saver.StepInfo[i].Origin, pos);

				VGSHelper::Get().DrawText("Step", pos.x - sz.x * 0.5f, pos.y, Color::White, 12);
			}
			/*else
			{
				g_Saver.StepInfo.erase(g_Saver.StepInfo.begin() + i);
			}*/
		}
	}

	/*C_BaseEntity* ent2 = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(g_Saver.StepInfo.UserID));
	if (ent != ent2 || g_Saver.StepInfo.Time > .5f)
		return;
	Vector pos;
	//auto bbox = GetBBox(ent->GetRenderOrigin);
	auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, "Step");
	//int w = bbox.right - bbox.left;
	Math::WorldToScreen(ent->GetRenderOrigin(), pos);

	VGSHelper::Get().DrawText("Step", pos.x - sz.x * 0.5f, pos.y, Color::White, 12);*/
}


void Visuals::DrawGrenade ( C_BaseEntity* ent )
{
    ClassId id = ent->GetClientClass()->m_ClassID;
    Vector vGrenadePos2D;
    Vector vGrenadePos3D = ent->m_vecOrigin();

    if ( !Math::WorldToScreen ( vGrenadePos3D, vGrenadePos2D ) )
        return;

    switch ( id )
    {
        case ClassId::CSmokeGrenadeProjectile:
            VGSHelper::Get().DrawText ( "smoke", vGrenadePos2D.x, vGrenadePos2D.y, Color::White, 12 );
            break;

        case ClassId::CBaseCSGrenadeProjectile:
        {
            model_t* model = ent->GetModel();

            if ( !model )
            {
                VGSHelper::Get().DrawText ( "nade", vGrenadePos2D.x, vGrenadePos2D.y, Color::White, 12 );
                return;
            }

            studiohdr_t* hdr = g_MdlInfo->GetStudiomodel ( model );

            if ( !hdr )
            {
                VGSHelper::Get().DrawText ( "nade", vGrenadePos2D.x, vGrenadePos2D.y, Color::White, 12 );
                return;
            }

            std::string name = hdr->szName;

            if ( name.find ( "incendiarygrenade" ) != std::string::npos || name.find ( "fraggrenade" ) != std::string::npos )
            {
                VGSHelper::Get().DrawText ( "frag", vGrenadePos2D.x, vGrenadePos2D.y, Color::Red, 12 );
                return;
            }

            VGSHelper::Get().DrawText ( "flash", vGrenadePos2D.x, vGrenadePos2D.y, Color::White, 12 );
            break;
        }

        case ClassId::CMolotovProjectile:
            VGSHelper::Get().DrawText ( "molo", vGrenadePos2D.x, vGrenadePos2D.y, Color::Red, 12 );
            break;

        case ClassId::CDecoyProjectile:
            VGSHelper::Get().DrawText ( "decoy", vGrenadePos2D.x, vGrenadePos2D.y, Color::White, 12 );
            break;
    }
}
void Visuals::DrawDangerzoneItem ( C_BaseEntity* ent, float maxRange )
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    ClientClass* cl = ent->GetClientClass();

    if ( !cl )
        return;

    ClassId id = cl->m_ClassID;
    //std::string name = cl->m_pNetworkName;

    std::string name = "unknown";

    const model_t* itemModel = ent->GetModel();

    if ( !itemModel )
        return;

    studiohdr_t* hdr = g_MdlInfo->GetStudiomodel ( itemModel );

    if ( !hdr )
        return;

    name = hdr->szName;



    if ( id != ClassId::CPhysPropAmmoBox && id != ClassId::CPhysPropLootCrate && id != ClassId::CPhysPropRadarJammer && id != ClassId::CPhysPropWeaponUpgrade )
        return;

    Vector vPos2D;
    Vector vPos3D = ent->m_vecOrigin();

    //vPos3D
    if ( g_LocalPlayer->m_vecOrigin().DistTo ( vPos3D ) > maxRange )
        return;

    if ( !Math::WorldToScreen ( vPos3D, vPos2D ) )
        return;



    if ( name.find ( "case_pistol" ) != std::string::npos )
        name = "pistol case";
    else if ( name.find ( "case_light_weapon" ) != std::string::npos ) // Reinforced!
        name = "light case";
    else if ( name.find ( "case_heavy_weapon" ) != std::string::npos )
        name = "heavy case";
    else if ( name.find ( "case_explosive" ) != std::string::npos )
        name = "explosive case";
    else if ( name.find ( "case_tools" ) != std::string::npos )
        name = "tools case";
    else if ( name.find ( "random" ) != std::string::npos )
        name = "airdrop";
    else if ( name.find ( "dz_armor_helmet" ) != std::string::npos )
        name = "full armor";
    else if ( name.find ( "dz_helmet" ) != std::string::npos )
        name = "helmet";
    else if ( name.find ( "dz_armor" ) != std::string::npos )
        name = "armor";
    else if ( name.find ( "upgrade_tablet" ) != std::string::npos )
        name = "tablet upgrade";
    else if ( name.find ( "briefcase" ) != std::string::npos )
        name = "briefcase";
    else if ( name.find ( "parachutepack" ) != std::string::npos )
        name = "parachute";
    else if ( name.find ( "dufflebag" ) != std::string::npos )
        name = "cash dufflebag";
    else if ( name.find ( "ammobox" ) != std::string::npos )
        name = "ammobox";

    VGSHelper::Get().DrawText ( name, vPos2D.x, vPos2D.y, Color::White, 12 );
}
//--------------------------------------------------------------------------------
void Visuals::ThirdPerson()
{
    if ( !g_LocalPlayer )
        return;

	if (Settings::Visual::ThirdPersonEnabled && g_LocalPlayer->IsAlive())
    {
        if ( !g_Input->m_fCameraInThirdPerson )
            g_Input->m_fCameraInThirdPerson = true;
    }
    else
        g_Input->m_fCameraInThirdPerson = false;
}

void Visuals::LbyIndicator()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    int x, y;
    g_EngineClient->GetScreenSize ( x, y );

    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );

    Color clr = Color::Green;

    if ( Moving && !InAir )
        clr = Color::Red;

    if ( fabs ( g_Saver.AARealAngle.yaw - g_LocalPlayer->m_flLowerBodyYawTarget() ) < 35.f )
        clr = Color::Red;

    if ( g_Saver.InFakewalk )
        clr = Color ( 255, 150, 0 );

    float percent;

    if ( Moving || InAir || g_Saver.InFakewalk )
        percent = 1.f;
    else
        percent = ( g_Saver.NextLbyUpdate - g_GlobalVars->curtime ) / 1.1f;

    percent = 1.f - percent;

    ImVec2 t = g_pDefaultFont->CalcTextSizeA ( 34.f, FLT_MAX, 0.0f, "LBY" );
    float width = t.x * percent;

    Render::Get().RenderLine ( 9.f, y - 100.f - ( CurrentIndicatorHeight - 34.f ), 11.f + t.x, y - 100.f - ( CurrentIndicatorHeight - 34.f ), Color ( 0, 0, 0, 25 ), 4.f );

    if ( width < t.x && width > 0.f )
        Render::Get().RenderLine ( 10.f, y - 100.f - ( CurrentIndicatorHeight - 34.f ), 10.f + width, y - 100.f - ( CurrentIndicatorHeight - 34.f ), clr, 2.f );

    Render::Get().RenderTextNoOutline ( "LBY", ImVec2 ( 10, y - 100.f - CurrentIndicatorHeight ), 34.f, clr );
    CurrentIndicatorHeight += 34.f;
}

void Visuals::PingIndicator()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    INetChannelInfo* nci = g_EngineClient->GetNetChannelInfo();

    if ( !nci )
        return;

    float ping = nci ? ( nci->GetAvgLatency ( FLOW_INCOMING ) ) * 1000.f : 0.0f;
    int x, y;
    g_EngineClient->GetScreenSize ( x, y );

    //std::string text = "PING: " + std::to_string(ping);
    float percent = ping / 100.f;
    ImVec2 t = g_pDefaultFont->CalcTextSizeA ( 34.f, FLT_MAX, 0.0f, "PING" );
    float width = t.x * percent;

    int green = int ( percent * 2.55f );
    int red = 255 - green;

    Render::Get().RenderLine ( 9.f, y - 100.f - ( CurrentIndicatorHeight - 34.f ), 11.f + t.x, y - 100.f - ( CurrentIndicatorHeight - 34.f ), Color ( 0, 0, 0, 25 ), 4.f );
    Render::Get().RenderLine ( 10.f, y - 100.f - ( CurrentIndicatorHeight - 34.f ), 10.f + width, y - 100.f - ( CurrentIndicatorHeight - 34.f ), Color ( red, green, 0 ), 2.f );
    Render::Get().RenderTextNoOutline ( "PING", ImVec2 ( 10, y - 100.f - CurrentIndicatorHeight ), 34.f, Color ( red, green, 0 ) );
    CurrentIndicatorHeight += 34.f;
}

void Visuals::LCIndicator()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() || g_LocalPlayer->m_fFlags() & FL_ONGROUND )
        return;

    int x, y;
    g_EngineClient->GetScreenSize ( x, y );

    if ( ( g_LocalPlayer->m_fFlags() & FL_ONGROUND ) )
        return;

    //ImVec2 t = g_pDefaultFont->CalcTextSizeA(34.f, FLT_MAX, 0.0f, "LBY");
    Render::Get().RenderTextNoOutline ( "LC", ImVec2 ( 10, y - 100.f - CurrentIndicatorHeight ), 34.f, g_Saver.LCbroken ? Color::Green : Color::Red );
    CurrentIndicatorHeight += 34.f;
}

void Visuals::BAimIndicator()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	Color clr = Color::Red;
	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	if (*Rbot::Get().GetBAimStatus() == BaimMode::FORCE_BAIM)
		clr = Color::Green;
	ImVec2 t = g_pDefaultFont->CalcTextSizeA(34.f, FLT_MAX, 0.0f, "BAIM");

	Render::Get().RenderTextNoOutline("BAIM", ImVec2(10, y - 100.f - CurrentIndicatorHeight), 34.f, clr);

}

void Visuals::AutowallCrosshair()
{
    /*
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;
    float Damage = 0.f;
    Autowall::Get().trace_awall(Damage);
    if (Damage != 0.f)
    {
    	int x, y;
    	g_EngineClient->GetScreenSize(x, y);

    	float cx = x / 2.f, cy = y / 2.f;

    	VGSHelper::Get().DrawText("Damage: "+std::to_string(Damage), cx, cy, Color::Green, 12);
    }
    */
}

void Visuals::ManualAAIndicator()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    int x, y;
    g_EngineClient->GetScreenSize ( x, y );
    float cx = x / 2.f;
    float cy = y / 2.f;

    /*
    Vertex_t verts[3];
    verts[0].Init(Vector2D(cx - 70, cy));
    verts[1].Init(Vector2D(cx - 50.f, cy - 15.f));
    verts[2].Init(Vector2D(cx - 50.f, cy + 15.f));
    VGSHelper::Get().DrawTriangle(3, verts, Color(225, 225, 225, 200));
    */

    /*
    auto draw_arrow = [&](float rot, Color color) -> void
    {
    	Vertex_t verts[3];
    	verts[0].Init(Vector2D(cx + cosf(rot) * 30.f, cy + sinf(rot) * 30.f));
    	verts[1].Init(Vector2D(cx + cosf(rot + degrees_to_radians(10)) * (30.f - 25.f), cy + sinf(rot + degrees_to_radians(10)) * (30.f - 25.f)));
    	verts[2].Init(Vector2D(cx + cosf(rot - degrees_to_radians(10)) * (30.f - 25.f), cy + sinf(rot - degrees_to_radians(10)) * (30.f - 25.f)));
    	VGSHelper::Get().DrawTriangle(3, verts, color);
    };
    */
    /*
    Vertex_t verts2[3];
    verts2[0].Init(Vector2D(cx + 50.f, cy));
    verts2[1].Init(Vector2D(cx + 70.f, cy - 15.f));
    verts2[2].Init(Vector2D(cx + 70.f, cy + 15.f));
    VGSHelper::Get().DrawTriangle(3, verts2, Color(0, 0, 0, 255));
    */

    //verts[1].Init(Vector2D(cx - 40.f, cy + 15.f));
    //verts[0].Init(Vector2D(cx - 30.f, cy + 15.f));
    //verts[2].Init(Vector2D(0, 0));
    //verts[3].Init(Vector2D(cx - 30.f, cy - 15.f));

    //Render::Get().render
    //Render::Get().RenderBoxFilled(cx - 30.f, cy - 15.f, cx - 15.f, cy + 15.f, Color::Blue);
    //Vertex_t* v = &Vertex_t(Vector2D(cx - 30.f, cy - 15.f), );
    //v[0].Init(Vector2D(cx - 30.f, cy - 15.f), Vector2D(cx - 15.f, cy + 15.f));
    //v[1].Init(Vector2D(cx + 30.f, cy - 15.f), Vector2D(cx + 15.f, cy + 15.f));
    //v->Init(Vector2D(15.f, cx), Vector2D(cx - 30.f, cy - 15.f));
    //VGSHelper::Get().DrawTriangle(25, v, Color::Green);
    //VGSHelper::Get().DrawFilledBox(cx - 30.f, cy - 15.f, cx - 30.f, cy + 15.f, Color::Blue);
    //VGSHelper::Get().DrawFilledBox(cx + 30.f, cy - 15.f, cx + 30.f, cy + 15.f, Color::Blue);
}

void Visuals::NoFlash()
{
    if ( !g_LocalPlayer )
        return;

    g_LocalPlayer->m_flFlashMaxAlpha() = 0.f;
}

void Visuals::SpreadCircle()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if ( !weapon )
        return;

    float spread = weapon->GetInaccuracy() * 1000;

    if ( spread == 0.f )
        return;

    //Console.WriteLine(spread);
    int x, y;
    g_EngineClient->GetScreenSize ( x, y );
    float cx = x / 2.f;
    float cy = y / 2.f;
    //VGSHelper::Get().DrawCircle ( cx, cy, spread, 35, g_Config.GetColor ( "vis_misc_draw_circle_clr" ) );
	VGSHelper::Get().DrawCircle(cx, cy, spread, 35, Settings::Visual::SpreadCircleColor);
    //Render::Get().RenderCircle(x, y, spread, 200, Color::Green, 1.f);
}

void Visuals::RenderNoScoopeOverlay()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    static int cx;
    static int cy;
    static int w, h;

    g_EngineClient->GetScreenSize ( w, h );
    cx = w / 2;
    cy = h / 2;

    if ( g_LocalPlayer->m_bIsScoped() )
    {
        VGSHelper::Get().DrawLine ( 0, cy, w, cy, Color::Black );
        VGSHelper::Get().DrawLine ( cx, 0, cx, h, Color::Black );
    }
}

void Visuals::RenderHitmarker()
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    static int cx;
    static int cy;
    static int w, h;

    g_EngineClient->GetScreenSize ( w, h );
    cx = w / 2;
    cy = h / 2;

    //g_Saver.HitmarkerInfo.HitTime
    if ( g_GlobalVars->realtime - g_Saver.HitmarkerInfo.HitTime > .5f )
        return;

    float percent = ( g_GlobalVars->realtime - g_Saver.HitmarkerInfo.HitTime ) / .5f;
    float percent2 = percent;

    if ( percent > 1.f )
    {
        percent = 1.f;
        percent2 = 1.f;
    }

    percent = 1.f - percent;
    float addsize = percent2 * 5.f;


    Color clr = Color ( 255, 255, 255, ( int ) ( percent * 255.f ) );

    VGSHelper::Get().DrawLine ( cx - 3.f - addsize, cy - 3.f - addsize, cx + 3.f + addsize, cy + 3.f + addsize, clr, 1.f );
    VGSHelper::Get().DrawLine ( cx - 3.f - addsize, cy + 3.f + addsize, cx + 3.f + addsize, cy - 3.f - addsize, clr, 1.f );
}

void Visuals::AddToDrawList()
{
    if ( !g_EngineClient->IsConnected() || !g_LocalPlayer || !g_EngineClient->IsInGame() )
        return;

	bool GrenadeEsp = Settings::Visual::GlobalESP.GrenadeEnabled; 
    DrawSideModes health_pos = ( DrawSideModes ) g_Config.GetInt ( "esp_health_pos" );
    DrawSideModes armour_pos = ( DrawSideModes ) g_Config.GetInt ( "esp_armour_pos" );

	bool esp_local_enabled = Settings::Visual::LocalESP.Enabled; 
	bool esp_team_enabled = Settings::Visual::TeamESP.Enabled;
	bool esp_enemy_enabled = Settings::Visual::EnemyESP.Enabled;

    bool esp_local_boxes = false;
    bool esp_local_weapons = false;
    bool esp_local_names = false;
    bool esp_local_health = false;
    bool esp_local_armour = false;
    int esp_local_boxes_type = 0;
    Color color_esp_local_boxes = Color ( 0, 0, 0 );
    Color color_esp_local_names = Color ( 0, 0, 0 );
    Color color_esp_local_armour = Color ( 0, 0, 0 );
    Color color_esp_local_weapons = Color ( 0, 0, 0 );

	if (esp_local_enabled && Settings::Visual::ThirdPersonEnabled)
    {
		esp_local_boxes = Settings::Visual::LocalESP.BoxEnabled;
		esp_local_weapons = Settings::Visual::LocalESP.WeaponEnabled;
		esp_local_names = Settings::Visual::LocalESP.NameEnabled;
		esp_local_health = Settings::Visual::LocalESP.HealthEnabled;
		esp_local_armour = Settings::Visual::LocalESP.ArmorEnabled;
		esp_local_boxes_type = Settings::Visual::LocalESP.BoxType;
		color_esp_local_boxes = Settings::Visual::LocalESP.BoxColor;
		color_esp_local_names = Settings::Visual::LocalESP.NameColor;
		color_esp_local_armour = Settings::Visual::LocalESP.ArmorColor;
		color_esp_local_weapons = Settings::Visual::LocalESP.WeaponColor;
    }

    bool esp_team_boxes = false;
    bool esp_team_snaplines = false;
    bool esp_team_weapons = false;
    bool esp_team_names = false;
    bool esp_team_health = false;
    bool esp_team_armour = false;
    int esp_team_boxes_type = 0;
    Color color_esp_team_boxes = Color ( 0, 0, 0 );
    Color color_esp_team_names = Color ( 0, 0, 0 );
    Color color_esp_team_armour = Color ( 0, 0, 0 );
    Color color_esp_team_weapons = Color ( 0, 0, 0 );
    Color color_esp_team_snaplines = Color ( 0, 0, 0 );

    if ( esp_team_enabled )
    {
		esp_team_boxes = Settings::Visual::TeamESP.BoxEnabled;
		esp_team_snaplines = Settings::Visual::TeamESP.SnaplineEnabled;
		esp_team_weapons = Settings::Visual::TeamESP.WeaponEnabled;
		esp_team_names = Settings::Visual::TeamESP.NameEnabled;
		esp_team_health = Settings::Visual::TeamESP.HealthEnabled;
		esp_team_armour = Settings::Visual::TeamESP.ArmorEnabled;
		esp_team_boxes_type = Settings::Visual::TeamESP.BoxType;
		color_esp_team_boxes = Settings::Visual::TeamESP.BoxColor;
		color_esp_team_names = Settings::Visual::TeamESP.NameColor;
		color_esp_team_armour = Settings::Visual::TeamESP.ArmorColor;
		color_esp_team_weapons = Settings::Visual::TeamESP.WeaponColor;
		color_esp_team_snaplines = Settings::Visual::TeamESP.SnaplineColor;
    }

    bool esp_enemy_boxes = false;
    bool esp_enemy_snaplines = false;
    bool esp_enemy_weapons = false;
    bool esp_enemy_names = false;
    bool esp_enemy_health = false;
    bool esp_enemy_armour = false;
    bool esp_enemy_info = false;
    bool esp_enemy_lby_timer = false;
    int esp_enemy_boxes_type = 0;
    Color color_esp_enemy_boxes = Color ( 0, 0, 0 );
    Color color_esp_enemy_names = Color ( 0, 0, 0 );
    Color color_esp_enemy_armour = Color ( 0, 0, 0 );
    Color color_esp_enemy_weapons = Color ( 0, 0, 0 );
    Color color_esp_enemy_snaplines = Color ( 0, 0, 0 );
    Color color_esp_enemy_info = Color ( 0, 0, 0 );
    Color color_esp_enemy_lby_timer = Color::Blue;

    if ( esp_enemy_enabled )
    {
		esp_enemy_boxes = Settings::Visual::EnemyESP.BoxEnabled;
		esp_enemy_snaplines = Settings::Visual::EnemyESP.SnaplineEnabled;
		esp_enemy_weapons = Settings::Visual::EnemyESP.WeaponEnabled;
		esp_enemy_names = Settings::Visual::EnemyESP.NameEnabled;
		esp_enemy_health = Settings::Visual::EnemyESP.HealthEnabled;
		esp_enemy_armour = Settings::Visual::EnemyESP.ArmorEnabled;
		esp_enemy_boxes_type = Settings::Visual::EnemyESP.BoxType;
		esp_enemy_info = false;
		esp_enemy_lby_timer = false;
		color_esp_enemy_boxes = Settings::Visual::EnemyESP.BoxColor;
		color_esp_enemy_names = Settings::Visual::EnemyESP.NameColor;
		color_esp_enemy_armour = Settings::Visual::EnemyESP.ArmorColor;
		color_esp_enemy_weapons = Settings::Visual::EnemyESP.WeaponColor;
		color_esp_enemy_snaplines = Settings::Visual::EnemyESP.SnaplineColor;
		color_esp_enemy_info = Color::White;
		color_esp_enemy_lby_timer = Color::White; 
    }

    bool esp_misc_dangerzone_item_esp = false;
    float esp_misc_dangerzone_item_esp_dist = 0.f;
    #ifdef _DEBUG
    bool misc_debug_overlay = g_Config.GetBool ( "misc_debug_overlay" );
    #endif // _DEBUG
    bool IsDangerzone = g_LocalPlayer && g_LocalPlayer->InDangerzone();

    if ( IsDangerzone )
    {
		esp_misc_dangerzone_item_esp = Settings::Visual::GlobalESP.DZEnabled;
		esp_misc_dangerzone_item_esp_dist = Settings::Visual::GlobalESP.DZRange;
    }

    //bool esp_outline = g_Config.GetBool("esp_misc_outline");
	bool rbot_resolver = Settings::RageBot::Resolver; 

    bool esp_dropped_weapons = g_Config.GetBool ( "esp_dropped_weapons" );
    bool esp_planted_c4 = g_Config.GetBool ( "esp_planted_c4" );

    for ( auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i )
    {
        auto entity = C_BaseEntity::GetEntityByIndex ( i );

        if ( !entity )
            continue;

        if ( i < 65 && ( esp_local_enabled || esp_team_enabled || esp_enemy_enabled ) )
        {
            auto player = Player();

            if ( player.Begin ( ( C_BasePlayer* ) entity ) )
            {
                bool Enemy = player.ctx.pl->IsEnemy();
                bool Local = player.ctx.pl == g_LocalPlayer;
                bool Team = Team = !Enemy && !Local;

                if ( Local )
                {
                    player.ctx.BoxClr = color_esp_local_boxes;
                    player.ctx.NameClr = color_esp_local_names;
                    player.ctx.ArmourClr = color_esp_local_armour;
                    player.ctx.WeaponClr = color_esp_local_weapons;
                }
                else if ( Team )
                {
                    player.ctx.BoxClr = color_esp_team_boxes;
                    player.ctx.NameClr = color_esp_team_names;
                    player.ctx.ArmourClr = color_esp_team_armour;
                    player.ctx.WeaponClr = color_esp_team_weapons;
                    player.ctx.SnaplineClr = color_esp_team_snaplines;
                }
                else
                {
                    player.ctx.BoxClr = color_esp_enemy_boxes;
                    player.ctx.NameClr = color_esp_enemy_names;
                    player.ctx.ArmourClr = color_esp_enemy_armour;
                    player.ctx.WeaponClr = color_esp_enemy_weapons;
                    player.ctx.SnaplineClr = color_esp_enemy_snaplines;
                    player.ctx.InfoClr = color_esp_enemy_info;
                    player.ctx.LbyTimerClr = color_esp_enemy_lby_timer;
                }

                if ( Enemy )
                    player.ctx.boxmode = esp_enemy_boxes_type;
                else if ( Local )
                    player.ctx.boxmode = esp_local_boxes_type;
                else
                    player.ctx.boxmode = esp_team_boxes_type;

                player.ctx.healthpos = health_pos;
                player.ctx.armourpos = armour_pos;

                if ( ( Local && esp_local_enabled && esp_local_boxes ) || ( Team && esp_team_enabled && esp_team_boxes ) || ( Enemy && esp_enemy_enabled && esp_enemy_boxes ) )
                    player.RenderBox();

                if ( ( Team && esp_team_enabled && esp_team_snaplines ) || ( Enemy && esp_enemy_enabled && esp_enemy_snaplines ) )
                    player.RenderSnapline();

                if ( ( Local && esp_local_enabled && esp_local_names ) || ( Team && esp_team_enabled && esp_team_names ) || ( Enemy && esp_enemy_enabled && esp_enemy_names ) )
                    player.RenderName();

                if ( ( Local && esp_local_enabled && esp_local_health ) || ( Team && esp_team_enabled && esp_team_health ) || ( Enemy && esp_enemy_enabled && esp_enemy_health ) )
                    player.RenderHealth();

                if ( ( Local && esp_local_enabled && esp_local_armour ) || ( Team && esp_team_enabled && esp_team_armour ) || ( Enemy && esp_enemy_enabled && esp_enemy_armour ) )
                    player.RenderArmour();

                if ( rbot_resolver && Enemy && esp_enemy_info )
                    player.RenderResolverInfo();

                if ( ( Local && esp_local_enabled && esp_local_weapons ) || ( Team && esp_team_enabled && esp_team_weapons ) || ( Enemy && esp_enemy_enabled && esp_enemy_weapons ) )
                    player.RenderWeaponName();

                if ( Enemy && esp_enemy_lby_timer )
                    player.RenderLbyUpdateBar();

				/*if ((Enemy) || (Team))
					player.RenderSoundESP();*/

                #ifdef _DEBUG

                if ( misc_debug_overlay )
                    player.DrawPlayerDebugInfo();

                #endif // _DEBUG
            }
        }

        else if ( entity->IsWeapon() && esp_dropped_weapons )
            RenderWeapon ( static_cast<C_BaseCombatWeapon*> ( entity ) );
        else if ( entity->IsDefuseKit() && esp_dropped_weapons )
            RenderDefuseKit ( entity );
        else if ( entity->IsPlantedC4() && esp_planted_c4 )
            RenderPlantedC4 ( entity );

        if ( IsDangerzone && esp_misc_dangerzone_item_esp )
            DrawDangerzoneItem ( entity, esp_misc_dangerzone_item_esp_dist );

        // grenade esp
        if ( GrenadeEsp )
            DrawGrenade ( entity );

		RenderSoundESP();
    }

	if ( Settings::RageBot::Enabled )
    {
        LbyIndicator();
        PingIndicator();
		BAimIndicator();

        if ( g_Config.GetInt ( "misc_fakelag_mode" ) == 1 )
            LCIndicator();
    }

    if ( g_Config.GetBool ( "vis_misc_autowall_crosshair" ) )
        AutowallCrosshair();

    #ifdef _DEBUG
    ManualAAIndicator();
    #endif // _DEBUG

	if ( Settings::Visual::SpreadCircleEnabled )
        SpreadCircle();

    //if (g_Config.GetBool("esp_misc_bullettracer")) RenderBullettracers();
    //if (g_Config.GetBool("vis_misc_noflash")) NoFlash();

	if ( Settings::Visual::NoScopeOverlay )
        RenderNoScoopeOverlay();

	if ( Settings::Visual::Hitmarker )
        RenderHitmarker();

    CurrentIndicatorHeight = 0.f;
    //if (g_Config.GetBool("esp_crosshair"))
    //	RenderCrosshair();
}

void VGSHelper::Init()
{
    for ( int size = 1; size < 128; size++ )
    {
        fonts[size] = g_VGuiSurface->CreateFont_();
        g_VGuiSurface->SetFontGlyphSet ( fonts[size], "Sans-serif", size, 700, 0, 0, FONTFLAG_DROPSHADOW );
    }

    Inited = true;
}

void VGSHelper::DrawText ( std::string text, float x, float y, Color color, int size )
{
    if ( !Inited )
        Init();

    //int size = text.size() + 1;
    g_VGuiSurface->DrawClearApparentDepth();
    wchar_t buf[256];
    g_VGuiSurface->DrawSetTextFont ( fonts[size] );
    g_VGuiSurface->DrawSetTextColor ( color );

    if ( MultiByteToWideChar ( CP_UTF8, 0, text.c_str(), -1, buf, 256 ) )
    {
        g_VGuiSurface->DrawSetTextPos ( x, y );
        g_VGuiSurface->DrawPrintText ( buf, wcslen ( buf ) );
    }
}
void VGSHelper::DrawLine ( float x1, float y1, float x2, float y2, Color color, float size )
{
    /*
    if (outline) {
    	g_VGuiSurface->DrawSetColor(Color::Black);
    	//g_VGuiSurface->DrawSetApparentDepth(size + 1.f);
    	//g_VGuiSurface->DrawLine(x1, y1, x2, y2);
    	g_VGuiSurface->DrawFilledRect(x1 - size, y1 - size, x2 + size, y2 + size);
    }
    */
    g_VGuiSurface->DrawSetColor ( color );

    if ( size == 1.f )
        g_VGuiSurface->DrawLine ( x1, y1, x2, y2 );
    else
        g_VGuiSurface->DrawFilledRect ( x1 - ( size / 2.f ), y1 - ( size / 2.f ), x2 + ( size / 2.f ), y2 + ( size / 2.f ) );
}
void VGSHelper::DrawBox ( float x1, float y1, float x2, float y2, Color clr, float size )
{
    /*
    if (outline) {
    	g_VGuiSurface->DrawSetColor(Color::Black);
    	g_VGuiSurface->DrawFilledRect(x1 - 1.f, y1, x1 + 1.f, y2); // left
    	g_VGuiSurface->DrawFilledRect(x2 - 1.f, y1, x2 + 1.f, y2); // right
    	g_VGuiSurface->DrawFilledRect(x1, y1 - 1.f, x2, y1 + 1.f); // top
    	g_VGuiSurface->DrawFilledRect(x1, y2 - 1.f, x2, y2 + 1.f); // bottom
    }
    */
    //g_VGuiSurface->DrawSetColor(clr);
    //g_VGuiSurface->DrawSetApparentDepth(size);
    //g_VGuiSurface->DrawOutlinedRect(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2));
    DrawLine ( x1, y1, x2, y1, clr, size );
    DrawLine ( x1, y2, x2, y2, clr, size );
    DrawLine ( x1, y1, x1, y2, clr, size );
    DrawLine ( x2, y1, x2, y2, clr, size );

}
void VGSHelper::DrawFilledBox ( float x1, float y1, float x2, float y2, Color clr )
{
    g_VGuiSurface->DrawSetColor ( clr );
    //g_VGuiSurface->DrawSetApparentDepth(size);
    g_VGuiSurface->DrawFilledRect ( static_cast<int> ( x1 ), static_cast<int> ( y1 ), static_cast<int> ( x2 ), static_cast<int> ( y2 ) );

}
void VGSHelper::DrawTriangle ( int count, Vertex_t* vertexes, Color c )
{
    static int Texture = g_VGuiSurface->CreateNewTextureID ( true ); // need to make a texture with procedural true
    unsigned char buffer[4] = { ( unsigned char ) c.r(), ( unsigned char ) c.g(), ( unsigned char ) c.b(), ( unsigned char ) c.a() }; // r,g,b,a

    g_VGuiSurface->DrawSetTextureRGBA ( Texture, buffer, 1, 1 ); //Texture, char array of texture, width, height
    g_VGuiSurface->DrawSetColor ( c ); // keep this full color and opacity use the RGBA @top to set values.
    g_VGuiSurface->DrawSetTexture ( Texture ); // bind texture

    g_VGuiSurface->DrawTexturedPolygon ( count, vertexes );
}
void VGSHelper::DrawBoxEdges ( float x1, float y1, float x2, float y2, Color clr, float edge_size, float size )
{
    if ( fabs ( x1 - x2 ) < ( edge_size * 2 ) )
    {
        //x2 = x1 + fabs(x1 - x2);
        edge_size = fabs ( x1 - x2 ) / 4.f;
    }

    DrawLine ( x1, y1, x1, y1 + edge_size + ( 0.5f * edge_size ), clr, size );
    DrawLine ( x2, y1, x2, y1 + edge_size + ( 0.5f * edge_size ), clr, size );
    DrawLine ( x1, y2, x1, y2 - edge_size - ( 0.5f * edge_size ), clr, size );
    DrawLine ( x2, y2, x2, y2 - edge_size - ( 0.5f * edge_size ), clr, size );
    DrawLine ( x1, y1, x1 + edge_size, y1, clr, size );
    DrawLine ( x2, y1, x2 - edge_size, y1, clr, size );
    DrawLine ( x1, y2, x1 + edge_size, y2, clr, size );
    DrawLine ( x2, y2, x2 - edge_size, y2, clr, size );
}
void VGSHelper::DrawCircle ( float x, float y, float r, int seg, Color clr )
{
    g_VGuiSurface->DrawSetColor ( clr );
    g_VGuiSurface->DrawOutlinedCircle ( x, y, r, seg );
}
ImVec2 VGSHelper::GetSize ( std::string text, int size )
{
    if ( !Inited )
        Init();

    wchar_t buf[256];
    int x, y;

    if ( MultiByteToWideChar ( CP_UTF8, 0, text.c_str(), -1, buf, 256 ) )
    {
        g_VGuiSurface->GetTextSize ( fonts[size], buf, x, y );
        return ImVec2 ( x, y );
    }

    return ImVec2 ( 0, 0 );
}