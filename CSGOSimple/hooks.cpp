
#include "hooks.hpp"
#include <intrin.h>
#include "options.hpp"
#include "render.hpp"
#include "menu_helpers.hpp"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "EnginePrediction.h"
#include "Chams.h"
#include "MovementFix.h"
#include "Rbot.h"
#include "KeyLoop.h"
#include "ThirdpersonAngleHelper.h"
#include "AntiAim.h"
#include "Fakelag.h"
#include "Resolver.h"
#include "BuyBot.h"
#include "HitPossitionHelper.h"
#include "RuntimeSaver.h"
#include "Logger.h"
#include "ClantagChanger.h"
#include "Backtrack.h"
#include "features/MaterialManager.hpp"
#include "Lbot.h"
#include "Misc.h"
#include "ConsoleHelper.h"
#include "Utils\ConvarSpoofer.h"
#include "features\TriggerBot.h"
#include "Settings.h"
#include "features\NightMode.h"
#include "features\Skinchanger.h"
#include "features\GrenadeTrajectory.h"
#include "features\Radar.h"
//#include "Asuswalls.h"
#include "NoSmoke.h"
#pragma intrinsic(_ReturnAddress)

namespace Hooks
{
    vfunc_hook hlclient_hook;
    vfunc_hook direct3d_hook;
    vfunc_hook vguipanel_hook;
    vfunc_hook vguisurf_hook;
    vfunc_hook sound_hook;
    vfunc_hook mdlrender_hook;
    vfunc_hook clientmode_hook;
    vfunc_hook sv_cheats;
	recv_prop_hook* sequence_hook;
    vfunc_hook RenderView_hook;
    vfunc_hook ViewRender_hook;
    vfunc_hook gameevents_hook;
    vfunc_hook clientstate_hook;

	float flAngle = 0.f;

	CUtlVector<SndInfo_t> sndList;

    void Initialize()
    {
        g_Logger.Clear();
        g_Logger.Info ( "cheat", "initializing cheat" );

        hlclient_hook.setup ( g_CHLClient, "client_panorama.dll" );
        direct3d_hook.setup ( g_D3DDevice9, "shaderapidx9.dll" );
        vguipanel_hook.setup ( g_VGuiPanel, "vgui2.dll" );
        vguisurf_hook.setup ( g_VGuiSurface, "vguimatsurface.dll" );
        sound_hook.setup ( g_EngineSound, "engine.dll" );
        mdlrender_hook.setup ( g_MdlRender, "client_panorama.dll" );
        clientmode_hook.setup ( g_ClientMode, "client_panorama.dll" );
        ConVar* sv_cheats_con = g_CVar->FindVar ( "sv_cheats" );
        sv_cheats.setup ( sv_cheats_con );
        RenderView_hook.setup ( g_RenderView, "engine.dll" );
        gameevents_hook.setup ( g_GameEvents, "engine.dll" );
        ViewRender_hook.setup ( g_ViewRender, "client_panorama.dll" );
		sequence_hook = new recv_prop_hook(C_BaseViewModel::m_nSequence(), hkRecvProxy);

        direct3d_hook.hook_index ( index::EndScene, hkEndScene );
        direct3d_hook.hook_index ( index::Reset, hkReset );

        hlclient_hook.hook_index ( index::FrameStageNotify, hkFrameStageNotify );
        hlclient_hook.hook_index ( index::CreateMove, hkCreateMove_Proxy );

        vguipanel_hook.hook_index ( index::PaintTraverse, hkPaintTraverse );

        sound_hook.hook_index ( index::EmitSound1, hkEmitSound1 );
        vguisurf_hook.hook_index ( index::LockCursor, hkLockCursor );

        mdlrender_hook.hook_index ( index::DrawModelExecute, hkDrawModelExecute );

        clientmode_hook.hook_index ( index::DoPostScreenSpaceEffects, hkDoPostScreenEffects );
        clientmode_hook.hook_index ( index::OverrideView, hkOverrideView );

        sv_cheats.hook_index ( index::SvCheatsGetBool, hkSvCheatsGetBool );

        RenderView_hook.hook_index ( index::SceneEnd, hkSceneEnd );
        ViewRender_hook.hook_index ( index::SmokeOverlay, Hooked_RenderSmokeOverlay );
        gameevents_hook.hook_index ( index::FireEvent, hkFireEvent );

        g_Logger.Success ( "cheat", "cheat initialized" );

        #ifdef _DEBUG
        NetvarSys::Get().Dump();
        #endif // _DEBUG

        g_CVar->FindVar ( "cl_interpolate" )->SetValue ( 1 ); //0
        g_CVar->FindVar ( "sv_showanimstate" )->SetValue ( 1 );
        g_CVar->FindVar ( "developer" )->SetValue ( 0 );
        g_CVar->FindVar ( "cl_interp_ratio" )->SetValue ( 2 );
        //g_CVar->FindVar("con_filter_text_out")->SetValue("");
        //g_CVar->FindVar("con_filter_enable")->SetValue(2);
        //g_CVar->FindVar("con_filter_text")->SetValue(".     ");
        //g_CVar->FindVar("con_notifytime")->SetValue(3);

        //g_CVar->FindVar("cl_interp_ratio")->SetValue(1.0f);
        //g_CVar->FindVar("cl_smooth")->SetValue(0.0f);
        //g_CVar->FindVar("cl_smoothtime")->SetValue(0.01f);
        //g_CVar->FindVar("cl_lagcompensation")->SetValue(1);//cl_sv_lagcompensateself->SetValue(1);
    }
    //--------------------------------------------------------------------------------
    void Shutdown()
    {
        //clientstate_hook.unhook_all();

        Glow::Get().Shutdown();
        hlclient_hook.unhook_all();
        direct3d_hook.unhook_all();
        vguipanel_hook.unhook_all();
        vguisurf_hook.unhook_all();
        sound_hook.unhook_all();
        mdlrender_hook.unhook_all();
        clientmode_hook.unhook_all();
        //sv_cheats.unhook_all();
        RenderView_hook.unhook_all();
        ViewRender_hook.unhook_all();
        gameevents_hook.unhook_all();
		sequence_hook->~recv_prop_hook();

    }

	void UpdateSpoofer()
	{
		if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		{
			if (g_CVarSpoofer->Ready())
				g_CVarSpoofer->Release();

			return;
		}

		if (!g_CVarSpoofer->Ready()) 
		{
			g_CVarSpoofer->Init();

			g_CVarSpoofer->Add("sv_cheats");
			g_CVarSpoofer->Add("r_drawspecificstaticprop");

			g_CVarSpoofer->Spoof();
		}

		if (g_CVarSpoofer->Ready())
			g_CVarSpoofer->Get("sv_cheats")->SetInt(1);
	}
    //--------------------------------------------------------------------------------
    bool HookedNetchan = false;
    long __stdcall hkEndScene ( IDirect3DDevice9* pDevice )
    {
        auto oEndScene = direct3d_hook.get_original<EndScene> ( index::EndScene );

        //if (g_Unload) clientstate_hook.unhook_all();

        if ( g_Unload )
            oEndScene ( pDevice );

        /*
        DWORD NetChannel = *(DWORD*)(*(DWORD*)g_ClientState + 0x9C);
        if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !NetChannel)
        {
        	if(HookedNetchan) clientstate_hook.unhook_all();
        	HookedNetchan = false;
        }
        */

        static auto viewmodel_fov = g_CVar->FindVar ( "viewmodel_fov" );
        static auto mat_ambient_light_r = g_CVar->FindVar ( "mat_ambient_light_r" );
        static auto mat_ambient_light_g = g_CVar->FindVar ( "mat_ambient_light_g" );
        static auto mat_ambient_light_b = g_CVar->FindVar ( "mat_ambient_light_b" );
        static auto crosshair_cvar = g_CVar->FindVar ( "crosshair" );
        static auto phys_pushscale_cvar = g_CVar->FindVar ( "phys_pushscale" );
        static auto phys_pushscale_org = phys_pushscale_cvar->Get<int>();
        static auto engine_no_focus_sleep_cvar = g_CVar->FindVar ( "engine_no_focus_sleep" );
        engine_no_focus_sleep_cvar->SetValue ( 0 );
        //static auto cl_extrapolate_cvar = g_CVar->FindVar("cl_extrapolate");//->SetValue(0);
        //cl_extrapolate_cvar->m_fnChangeCallbacks.m_Size = 0;
        //cl_extrapolate_cvar->SetValue(0);
        //static auto cl_interp_ratio_cvar = g_CVar->FindVar("cl_interp_ratio");
        //cl_interp_ratio
        //cl_interp
        //static auto cl_lagcompensation_cvar = g_CVar->FindVar("cl_lagcompensation");cl_interpolation
        //static auto cl_interp_cvar = g_CVar->FindVar("cl_interp");
        //static auto cl_sv_lagcompensateself = g_CVar->FindVar("sv_lagcompensateself");

        //cl_interp_ratio_cvar->SetValue(0);

        AntiAim::Get().ResetLbyPrediction();

        //test
        //cl_sv_lagcompensateself->SetValue(1);

        /*
        static bool setup = false;
        if (!setup)
        {
        	setup = true;
        	g_CVar->FindVar("cl_interp")->SetValue(0.01f);
        	g_CVar->FindVar("cl_cmdrate")->SetValue(66);
        	g_CVar->FindVar("cl_updaterate")->SetValue(66);
        	g_CVar->FindVar("cl_interp_all")->SetValue(0.0f);
        	g_CVar->FindVar("cl_interp_ratio")->SetValue(1.0f);
        	g_CVar->FindVar("cl_smooth")->SetValue(0.0f);
        	g_CVar->FindVar("cl_smoothtime")->SetValue(0.01f);
        }
        */

        #ifdef _DEBUG
        phys_pushscale_cvar->m_fnChangeCallbacks.m_Size = 0;
        phys_pushscale_cvar->SetValue ( phys_pushscale_org + ( 250 * g_Config.GetInt ( "vis_misc_addforce" ) ) );
        #endif // _DEBUG
        viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
        //viewmodel_fov->SetValue ( g_Config.GetInt ( "viewmodel_fov" ) ); //phys_pushscale
		viewmodel_fov->SetValue(Settings::Visual::ViewModelFOV); //phys_pushscale
        mat_ambient_light_r->SetValue ( g_Config.GetFloat ( "mat_ambient_light_r" ) );
        mat_ambient_light_g->SetValue ( g_Config.GetFloat ( "mat_ambient_light_g" ) );
        mat_ambient_light_b->SetValue ( g_Config.GetFloat ( "mat_ambient_light_b" ) );
        crosshair_cvar->SetValue ( !g_Config.GetBool ( "esp_crosshair" ) );
        //cl_interpolate_cvar->SetValue(1);
        //cl_lagcompensation_cvar->SetValue(1);
        //cl_interp_cvar->SetValue(1);

        DWORD colorwrite, srgbwrite;
        pDevice->GetRenderState ( D3DRS_COLORWRITEENABLE, &colorwrite );
        pDevice->GetRenderState ( D3DRS_SRGBWRITEENABLE, &srgbwrite );

        pDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, 0xffffffff );
        //removes the source engine color correction
        pDevice->SetRenderState ( D3DRS_SRGBWRITEENABLE, false );

        pDevice->SetSamplerState ( NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        pDevice->SetSamplerState ( NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        pDevice->SetSamplerState ( NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
        pDevice->SetSamplerState ( NULL, D3DSAMP_SRGBTEXTURE, NULL );

        ImGui_ImplDX9_NewFrame();

        auto esp_drawlist = Render::Get().RenderScene();

        MenuHelper::Get().Render();
		// Add radar render
		Radar::Get().Render();

        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData ( ImGui::GetDrawData(), esp_drawlist );

        pDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, colorwrite );
        pDevice->SetRenderState ( D3DRS_SRGBWRITEENABLE, srgbwrite );

        return oEndScene ( pDevice );
    }
    //--------------------------------------------------------------------------------
    long __stdcall hkReset ( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters )
    {
        auto oReset = direct3d_hook.get_original<Reset> ( index::Reset );

        MenuHelper::Get().OnDeviceLost();

        auto hr = oReset ( device, pPresentationParameters );

        if ( hr >= 0 )
            MenuHelper::Get().OnDeviceReset();

        return hr;
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkCreateMove ( int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket )
    {
        auto oCreateMove = hlclient_hook.get_original<CreateMove> ( index::CreateMove );

        oCreateMove ( g_CHLClient, sequence_number, input_sample_frametime, active );

        auto cmd = g_Input->GetUserCmd ( sequence_number );
        auto verified = g_Input->GetVerifiedCmd ( sequence_number );

        //g_LocalPlayer fix
        //g_LocalPlayer = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));

        if ( !cmd || !cmd->command_number || !bSendPacket || g_Unload || !g_EngineClient || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() )
            return;

        //if (g_ClientState->m_nDeltaTick != -1) return;

        //Console.WriteLine(cmd->viewangles);
        /*
        DWORD NetChannel = *(DWORD*)(*(DWORD*)g_ClientState + 0x9C);
        if (NetChannel && g_EngineClient && g_ClientState && !HookedNetchan && g_LocalPlayer && g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
        {
        	auto netchan = *reinterpret_cast<INetChannel**>(reinterpret_cast<std::uintptr_t>(g_ClientState) + 0x9C);
        	clientstate_hook.setup(netchan, "engine.dll");
        	clientstate_hook.hook_index(46, SendDatagram_h);
        	HookedNetchan = true;
        }
        */

        QAngle OldViewangles = cmd->viewangles;
        float OldForwardmove = cmd->forwardmove;
        float OldSidemove = cmd->sidemove;
		flAngle = cmd->viewangles.yaw;

        KeyLoop::Get().OnCreateMove();

        prediction->run_prediction ( cmd );


        #ifdef _DEBUG
        //Backtrack::Get().OnCreateMove();
        #endif // _DEBUG

		bool rbot = Settings::RageBot::Enabled;

        Misc::Get().OnCreateMove ( cmd );

		if(Settings::Misc::BHop)
            BunnyHop::Get().OnCreateMove ( cmd );

        if ( rbot )
            Fakelag::Get().OnCreateMove ( cmd, bSendPacket );

		if ( Settings::RageBot::EnabledAA )
            AntiAim::Get().OnCreateMove ( cmd, bSendPacket );

		//Lbot::Get().LegitAA(cmd, bSendPacket);

        if ( rbot )
        {
            Rbot::Get().PrecacheShit();
            Rbot::Get().CreateMove ( cmd, bSendPacket );
        }

		GrenadeHint::Get().Tick(cmd->buttons);

		if ( !rbot && Settings::Aimbot::Enabled )
        {
            Lbot::Get().OnCreateMove ( cmd );

			if(Settings::Aimbot::Backtrack)
                Backtrack::Get().FinishLegitBacktrack ( cmd );
        }

		if ( !rbot && Settings::TriggerBot::Enabled )
			TriggerBot::Get().OnCreateMove(cmd);

		if ( rbot && Settings::RageBot::Resolver )
            Resolver::Get().OnCreateMove ( OldViewangles );

        //if ( g_Config.GetBool ( "misc_buybot" ) )
		if ( Settings::Misc::BuyBot )
            BuyBot::Get().OnCreateMove();

		if ( Settings::Misc::Clantag )
            ClantagChanger::Get().OnCreateMove();

        prediction->end_prediction ( cmd );

        if ( g_LocalPlayer && g_LocalPlayer->IsAlive() && ( cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2 ) )
            g_Saver.LastShotEyePos = g_LocalPlayer->GetEyePos();

        if ( g_Saver.RbotDidLastShot )
        {
            g_Saver.RbotDidLastShot = false;

            if ( bSendPacket )
                bSendPacket = false;
        }

        //OldViewangles.pitch = cmd->viewangles.pitch;

        AntiAim::Get().LbyBreakerPrediction ( cmd, bSendPacket );

        //QAngle oldVang = cmd->viewangles;
        Math::NormalizeAngles ( cmd->viewangles );

        if ( g_LocalPlayer && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && g_LocalPlayer->m_nMoveType() != MOVETYPE_NOCLIP )
        {
			if ( Settings::Misc::AutoStrafe )
                BunnyHop::Get().AutoStrafe ( cmd, OldViewangles );
            else
                MovementFix::Get().Correct ( OldViewangles, cmd, OldForwardmove, OldSidemove );
        }

        //if ( g_Config.GetBool ( "rbot_slidewalk" ) )
		if ( Settings::RageBot::SlideWalk )
            AntiAim::Get().SlideWalk ( cmd );

        Math::ClampAngles ( cmd->viewangles );

        verified->m_cmd = *cmd;
        verified->m_crc = cmd->GetChecksum();
    }
    //--------------------------------------------------------------------------------
    __declspec ( naked ) void __stdcall hkCreateMove_Proxy ( int sequence_number, float input_sample_frametime, bool active )
    {
        __asm
        {
            push ebp
            mov  ebp, esp
            push ebx
            lea  ecx, [esp]
            push ecx
            push dword ptr[active]
            push dword ptr[input_sample_frametime]
            push dword ptr[sequence_number]
            call Hooks::hkCreateMove
            pop  ebx
            pop  ebp
            retn 0Ch
        }
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkPaintTraverse ( vgui::VPANEL panel, bool forceRepaint, bool allowForce )
    {
        static auto panelId = vgui::VPANEL{ 0 };
        static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse> ( index::PaintTraverse );

		if( Settings::Visual::NoScopeOverlay && !strcmp("HudZoom", g_VGuiPanel->GetName(panel)) )
            return;

        oPaintTraverse ( g_VGuiPanel, panel, forceRepaint, allowForce );

        if ( g_Unload )
            return;

		static bool* s_bOverridePostProcessingDisable = *(bool**)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "80 3D ? ? ? ? ? 53 56 57 0F 85") + 0x2);
		*s_bOverridePostProcessingDisable = Settings::Visual::DisablePP;

        if ( !panelId )
        {
            const auto panelName = g_VGuiPanel->GetName ( panel );

            if ( !strcmp ( panelName, "FocusOverlayPanel" ) )
                panelId = panel;
        }
        else if ( panelId == panel )
        {
            //Ignore 50% cuz it called very often
            static bool bSkip = false;
            bSkip = !bSkip;

            if ( bSkip )
                return;

			if ( g_LocalPlayer && InputSys::Get().IsKeyDown(VK_TAB) && Settings::Misc::RankReveal )
                Utils::RankRevealAll();

            Render::Get().BeginScene();
			GrenadeHint::Get().Paint();
			//if ( g_LocalPlayer && flAngle )
			//	Visuals::Get().DrawEnemyCircle(flAngle);
        }
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkEmitSound1 ( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk )
    {
        static auto ofunc = sound_hook.get_original<EmitSound1> ( index::EmitSound1 );

		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep"))
			Misc::Get().SetLocalPlayerReady();

        ofunc ( g_EngineSound, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk );
    }
    //--------------------------------------------------------------------------------
    int __stdcall hkDoPostScreenEffects ( int a1 )
    {
        auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects> ( index::DoPostScreenSpaceEffects );

        if ( g_LocalPlayer && g_Config.GetBool ( "glow_enabled" ) && !g_Unload && g_EngineClient->IsConnected() && g_EngineClient->IsInGame() )
            Glow::Get().Run();

        return oDoPostScreenEffects ( g_ClientMode, a1 );
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkFrameStageNotify ( ClientFrameStage_t stage )
    {
        static auto ofunc = hlclient_hook.get_original<FrameStageNotify> ( index::FrameStageNotify );

		Skinchanger::Get().OnFrameStageNotify(stage);

        //if (g_ClientState->m_nDeltaTick != -1) return  ofunc(g_CHLClient, stage);
        if ( !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() )
            return ofunc ( g_CHLClient, stage );

        if ( !g_Unload )
            Misc::Get().OnFrameStageNotify ( stage );

        switch ( stage )
        {
            case FRAME_UNDEFINED:
                break;

            case FRAME_START:
                break;

            case FRAME_NET_UPDATE_START:
                break;

            case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
            {
				break;
            }

            case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
            {
                if ( g_Unload )
                    return;

				bool rbot = Settings::RageBot::Enabled;

				if ( rbot && Settings::RageBot::Resolver )
                    Resolver::Get().OnFramestageNotify();

                NoSmoke::Get().OnFrameStageNotify();

                for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
                {
                    auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

                    if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity->IsDormant()
                            || !entity->IsAlive() )
                        continue;

                    VarMapping_t* map = entity->VarMapping();

                    if ( map )
                    {
                        for ( int j = 0; j < map->m_nInterpolatedEntries; j++ )
                            map->m_Entries[j].m_bNeedsToInterpolate = false;
                    }
                }

                break;
            }

            case FRAME_NET_UPDATE_END:
				// add Sound ESP check
				/*if (true)
				{
					sndList.RemoveAll();
					g_EngineSound->GetActiveSounds(sndList);
					for (size_t i = 0; i < sndList.Count(); i++)
					{
						SndInfo_t snd = sndList.Element(i);
						if (snd.m_nSoundSource)
						{
							C_BaseEntity* ent = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(snd.m_nSoundSource));

							if (!ent || ent->IsDormant())
								continue;

							if (ent == g_LocalPlayer)
								continue;

							if (snd.m_nChannel == 4)
							{
								if (snd.m_nGuid)
								{
									if (snd.m_bUpdatePositions)
									{
										g_Saver.StepInfo.push_back(StepInfoStruct{ *snd.m_pOrigin, g_GlobalVars->curtime + 1, 15, snd.m_nSoundSource });
									}
								}
							}
						}
					}
				}*/
                break;

            case FRAME_RENDER_START:
            {
                if ( !g_Unload )
                {
					//UpdateSpoofer();
					bool rbot = Settings::RageBot::Enabled;

					if ( rbot && Settings::RageBot::EnabledAA && Settings::Visual::ThirdPersonEnabled )
                    {
                        ThirdpersonAngleHelper::Get().SetThirdpersonAngle();
                        ThirdpersonAngleHelper::Get().AnimFix();

						if (!g_LocalPlayer->IsAlive())
							g_LocalPlayer->m_iObserverMode() = 5;
						
                        //bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f || ( cmd->sidemove != 0.f || cmd->forwardmove != 0.f );
                        //bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );

                        // if ( !Moving && !InAir )
                        //    g_LocalPlayer->m_fFlags = ACT_FLY;

                        //ThirdpersonAngleHelper::Get().SetThirdpersonAngle();
                    }
                    else
                    {
                        if ( g_LocalPlayer && g_LocalPlayer->IsAlive() )
                            g_LocalPlayer->m_bClientSideAnimation() = true;
                    }

                    auto old_curtime = g_GlobalVars->curtime;
                    auto old_frametime = g_GlobalVars->frametime;

                    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
                    {
                        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

                        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity->IsDormant()
                                || !entity->IsAlive() )
                            continue;

                        if ( rbot )
                        {
                            entity->InvalidateBoneCache();
                            ThirdpersonAngleHelper::Get().EnemyAnimationFix ( entity );
                        }
                        else
                            entity->m_bClientSideAnimation() = true;
                    }

                    g_GlobalVars->curtime = old_curtime;
                    g_GlobalVars->frametime = old_frametime;
                }

                break;
            }

            case FRAME_RENDER_END:
				if (Settings::Visual::NightMode)
					NightMode::Get().Apply(false);
				else
					NightMode::Get().Revert();
                break;
        }

        ofunc ( g_CHLClient, stage );
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkOverrideView ( CViewSetup* vsView )
    {
        static auto ofunc = clientmode_hook.get_original<OverrideView> ( index::OverrideView );

        if ( !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() )
            return ofunc ( g_ClientMode, vsView );

		GrenadeHint::Get().View();

        if ( g_EngineClient->IsInGame() && vsView )
            Visuals::Get().ThirdPerson();

		if ( g_LocalPlayer && g_LocalPlayer->m_bIsScoped() && !Settings::Visual::DisableScopeZoom )
            return ofunc ( g_ClientMode, vsView );

		vsView->fov = Settings::Visual::FOV;

        ofunc ( g_ClientMode, vsView );
    }
    //--------------------------------------------------------------------------------
    void __stdcall hkLockCursor()
    {
        static auto ofunc = vguisurf_hook.get_original<LockCursor_t> ( index::LockCursor );

        if ( MenuHelper::Get().IsVisible() )
        {
            g_VGuiSurface->UnlockCursor();
            return;
        }

        ofunc ( g_VGuiSurface );

    }
    //--------------------------------------------------------------------------------
    void __stdcall hkDrawModelExecute ( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld )
    {
        static auto ofunc = mdlrender_hook.get_original<DrawModelExecute> ( index::DrawModelExecute );
        //Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
        /*
        auto rotate_vec = [](vec3 _origin, float _yaw, float _dist) -> vec3
        {
        vec3 _dir;
        math::angle_vectors(q_ang(0, _yaw, 0), _dir);
        return _origin + (_dir * _dist);
        };

        vec3 _sim_pos = rotate_vec(m_localplayer->m_origin(), _angle, 18);
        */
        ofunc ( g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld );
        g_MdlRender->ForcedMaterialOverride ( nullptr );
    }

    auto dwCAM_Think = Utils::PatternScan ( GetModuleHandleW ( L"client_panorama.dll" ), "85 C0 75 30 38 86" );
    typedef bool ( __thiscall* svc_get_bool_t ) ( PVOID );
    bool __fastcall hkSvCheatsGetBool ( PVOID pConVar, void* edx )
    {
        static auto ofunc = sv_cheats.get_original<svc_get_bool_t> ( 13 );

        if ( !ofunc )
            return false;

        if ( reinterpret_cast<DWORD> ( _ReturnAddress() ) == reinterpret_cast<DWORD> ( dwCAM_Think ) )
            return true;

        return ofunc ( pConVar );
    }
    //--------------------------------------------------------------------------------
    void __fastcall hkSceneEnd ( void* pEcx, void* pEdx )
    {
        static auto ofunc = RenderView_hook.get_original<SceneEnd> ( index::SceneEnd );
        ofunc ( pEcx, pEdx );

        if ( !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() )
            return;

        if ( g_Unload )
            return;

        //if (g_ClientState->m_nDeltaTick != -1) return;
        // |
        // v
        // code here
        Chams::Get().OnSceneEnd();
    }
    //--------------------------------------------------------------------------------
	bool __stdcall hkFireEvent(IGameEvent* pEvent)
	{
		static auto oFireEvent = gameevents_hook.get_original<FireEvent>(index::FireEvent);

		if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
			return oFireEvent(g_GameEvents, pEvent);

		// -->
		Rbot::Get().OnFireEvent(pEvent);
		Resolver::Get().OnFireEvent(pEvent);

		if (!strcmp(pEvent->GetName(), "round_start"))
			BuyBot::Get().OnRoundStart();

		/*if (!strcmp(pEvent->GetName(), "player_footstep"))
		{
			g_Logger.Info("VISUAL", "Sound ESP call");
			Visuals::Get().RenderSoundESP(pEvent);
		}*/
			

        HitPossitionHelper::Get().OnFireEvent ( pEvent );

        return oFireEvent ( g_GameEvents, pEvent );
    }

	static auto random_sequence(const int low, const int high) -> int
	{
		return rand() % (high - low + 1) + low;
	}

	static auto fix_animation(const char* model, const int sequence) -> int
	{
		enum ESequence
		{
			SEQUENCE_DEFAULT_DRAW = 0,
			SEQUENCE_DEFAULT_IDLE1 = 1,
			SEQUENCE_DEFAULT_IDLE2 = 2,
			SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
			SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
			SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
			SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
			SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
			SEQUENCE_DEFAULT_LOOKAT01 = 12,
			SEQUENCE_BUTTERFLY_DRAW = 0,
			SEQUENCE_BUTTERFLY_DRAW2 = 1,
			SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
			SEQUENCE_BUTTERFLY_LOOKAT03 = 15,
			SEQUENCE_FALCHION_IDLE1 = 1,
			SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
			SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
			SEQUENCE_FALCHION_LOOKAT01 = 12,
			SEQUENCE_FALCHION_LOOKAT02 = 13,
			SEQUENCE_DAGGERS_IDLE1 = 1,
			SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
			SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
			SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
			SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,
			SEQUENCE_BOWIE_IDLE1 = 1,
		};
		if (strstr(model, "models/weapons/v_knife_butterfly.mdl")) {
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_falchion_advanced.mdl")) {
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence - 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_push.mdl")) {
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return sequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence + 2;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_survival_bowie.mdl")) 
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return sequence - 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_ursus.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
				break;
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
				break;
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_stiletto.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(12, 13);
				break;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_widowmaker.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(14, 15);
				break;
			}
		}
		else
			return sequence;
	}

	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output)
	{
		static auto original_fn = sequence_hook->get_original_function();
		const auto local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
		if (local && local->IsAlive())
		{
			const auto proxy_data = const_cast<CRecvProxyData*>(pData);
			const auto view_model = static_cast<C_BaseViewModel*>(entity);
			if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid())
			{
				const auto owner = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntityFromHandle(view_model->m_hOwner()));
				if (owner == g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()))
				{
					const auto view_model_weapon_handle = view_model->m_hWeapon();
					if (view_model_weapon_handle.IsValid())
					{
						const auto view_model_weapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(view_model_weapon_handle));
						if (view_model_weapon)
						{
							if (k_weapon_info.count(view_model_weapon->m_Item().m_iItemDefinitionIndex()))
							{
								auto original_sequence = proxy_data->m_Value.m_Int;
								const auto override_model = k_weapon_info.at(view_model_weapon->m_Item().m_iItemDefinitionIndex()).model;
								proxy_data->m_Value.m_Int = fix_animation(override_model, proxy_data->m_Value.m_Int);
							}
						}
					}
				}
			}
		}
		original_fn(pData, entity, output);
	}

    //--------------------------------------------------------------------------------
    void __stdcall Hooked_RenderSmokeOverlay ( bool unk ) { /* no need to call :) we want to remove the smoke overlay */ }
    //--------------------------------------------------------------------------------

    /*
    int __fastcall SendDatagram_h(INetChannel * netchan, void *, bf_write * datagram)
    {
    	static auto ofunc = clientstate_hook.get_original<SendDatagram_t>(index::SendDatagram);
    	return ofunc(netchan, datagram);
    	return;
    	int32_t reliable_state = netchan->m_nInReliableState;
    	int32_t sequencenr = netchan->m_nInSequenceNr;
    	int32_t outseqncenr = netchan->m_nOutSequenceNr;

    	/*
    	C_BasePlayer* local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));

    	if (local && local->IsAlive() && g_Saver.CurrentInLbyBreak)
    	{
    		//g_Logger.Add("m_fClearTime", std::to_string(netchan->m_fClearTime), Color::Blue); + going up (3.850775)
    		netchan->m_nOutSequenceNr *= 150;
    		g_Saver.CurrentInLbyBreak = false;
    	}


    	int ret = ofunc(netchan, datagram);

    	netchan->m_nInReliableState = reliable_state;
    	netchan->m_nInSequenceNr = sequencenr;
    	netchan->m_nOutSequenceNr = outseqncenr;

    	return ret;
    }
    */
}
