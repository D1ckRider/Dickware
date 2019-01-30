#pragma once

#include "../singleton.hpp"

#include "../render.hpp"
#include "../helpers/math.hpp"
#include "../valve_sdk/csgostructs.hpp"


#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define MAX_FLOWS 2

enum class DrawSideModes : int
{
	TOP,
	RIGHT,
	BOTTOM,
	LEFT
};

struct PosAdjustmentStruct
{
	int top = 4;
	int right = 4;
	int bottom = 4;
	int left = 4;
};

class Visuals : public Singleton<Visuals>
{
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();
public:
	class Player
	{
	public:
		struct
		{
			C_BasePlayer* pl;
			bool          is_enemy;
			bool          is_visible;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
			int			  boxmode = 0;
			DrawSideModes healthpos = DrawSideModes::LEFT;
			DrawSideModes armourpos = DrawSideModes::RIGHT;
			DrawSideModes lbyupdatepos = DrawSideModes::BOTTOM;
			PosAdjustmentStruct PosHelper;
			Color BoxClr = Color(0, 0, 0);
			Color NameClr = Color(0, 0, 0);
			Color ArmourClr = Color(0, 0, 0);
			Color WeaponClr = Color(0, 0, 0);
			Color SnaplineClr = Color(0, 0, 0);
			Color InfoClr = Color(0, 0, 0);
			Color LbyTimerClr = Color::Blue;
			bool ShouldDrawBox = true;
		} ctx;

		bool Begin(C_BasePlayer * pl);
		void RenderBox();
		void RenderName();
		void RenderWeaponName();

		void RenderHealth();
		void RenderArmour();
		void RenderLbyUpdateBar();

		void RenderSnapline();

		void DrawPlayerDebugInfo();

		void RenderLine(DrawSideModes mode, Color color, float percent);

		void RenderResolverInfo();

		float TextHeight = 12.f;
	};
	void RenderCrosshair();
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderDefuseKit(C_BaseEntity* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderSoundESP();
	//void RenderBullettracers();
	void DrawGrenade(C_BaseEntity* ent);
	void DrawDangerzoneItem(C_BaseEntity* ent, float maxRange);
	void ThirdPerson();

	/* Local indicators */
	void LbyIndicator();
	void PingIndicator();
	void LCIndicator();
	void BAimIndicator();

	void AutowallCrosshair();

	/* aa indicator */
	void ManualAAIndicator();

	/* local misc */
	void NoFlash();

	void SpreadCircle();

	void RenderNoScoopeOverlay();

	float CurrentIndicatorHeight = 0.f;

	void RenderHitmarker();

public:
	void AddToDrawList();
	void Render();
};

class VGSHelper : public Singleton<VGSHelper>
{
public:
	void Init();
	void DrawText(std::string text, float x, float y, Color color, int size = 15);
	void DrawLine(float x1, float y1, float x2, float y2, Color color, float size = 1.f);
	void DrawBox(float x1, float y1, float x2, float y2, Color clr, float size = 1.f);
	void DrawFilledBox(float x1, float y1, float x2, float y2, Color clr);
	void DrawTriangle(int count, Vertex_t* vertexes, Color c);
	void DrawBoxEdges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size = 1.f);
	void DrawCircle(float x, float y, float r, int seg, Color clr);

	ImVec2 GetSize(std::string text, int size = 15);
private:
	bool Inited = false;
	vgui::HFont fonts[128];
};