#pragma once

#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>

namespace index
{
    constexpr auto EmitSound1                = 5;
    constexpr auto EmitSound2                = 6;
    constexpr auto EndScene                  = 42;
    constexpr auto Reset                     = 16;
    constexpr auto PaintTraverse             = 41;
    constexpr auto CreateMove                = 22;
    constexpr auto PlaySound                 = 82;
    constexpr auto FrameStageNotify          = 37;
    constexpr auto DrawModelExecute          = 21;
    constexpr auto DoPostScreenSpaceEffects  = 44;
    constexpr auto SvCheatsGetBool           = 13;
    constexpr auto OverrideView              = 18;
    constexpr auto LockCursor                = 67;
    constexpr auto SceneEnd					 = 9; //9
    constexpr auto FireEvent				 = 9;
    constexpr auto SmokeOverlay				 = 41; //40
    constexpr auto SendDatagram				 = 46;
	constexpr auto FireBullets				 = 7;
	constexpr auto WriteUsercmdDeltaToBuffer = 23;
	constexpr auto SuppressLists = 16;
}

namespace Hooks
{
    void Initialize();
    void Shutdown();

    extern vfunc_hook hlclient_hook;
    extern vfunc_hook direct3d_hook;
    extern vfunc_hook vguipanel_hook;
    extern vfunc_hook vguisurf_hook;
    extern vfunc_hook mdlrender_hook;
    extern vfunc_hook viewrender_hook;
	extern vfunc_hook firebullets_hook;

    using EndScene             = long ( __stdcall*  ) ( IDirect3DDevice9* );
    using Reset                = long ( __stdcall*  ) ( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
    using CreateMove           = void ( __thiscall* ) ( IBaseClientDLL*, int, float, bool );
    using PaintTraverse        = void ( __thiscall* ) ( IPanel*, vgui::VPANEL, bool, bool );
    using EmitSound1           = void ( __thiscall* ) ( void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int );
	using FireBullets		   = void ( __thiscall* ) (C_TEFireBullets*, DataUpdateType_t);

    using FrameStageNotify     = void ( __thiscall* ) ( IBaseClientDLL*, ClientFrameStage_t );
    using PlaySound            = void ( __thiscall* ) ( ISurface*, const char* name );
    using LockCursor_t         = void ( __thiscall* ) ( ISurface* );
    using DrawModelExecute     = void ( __thiscall* ) ( IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
    using FireEvent            = bool ( __thiscall* ) ( IGameEventManager2*, IGameEvent* pEvent );
    using DoPostScreenEffects  = int  ( __thiscall* ) ( IClientMode*, int );
    using OverrideView		   = void ( __thiscall* ) ( IClientMode*, CViewSetup* );

    using SceneEnd			   = void ( __fastcall* ) ( void*, void* );
    using FireEvent			   = bool ( __thiscall* ) ( IGameEventManager2*, IGameEvent* pEvent );
    using RenderSmokeOverlay_t = void ( __thiscall* ) ( IVRenderView*, bool );
    using SendDatagram_t	   = int  ( __thiscall* ) ( INetChannel*, bf_write* );
	using WriteUsercmdDeltaToBuffer_t = bool(__thiscall* )(IBaseClientDLL*, int, bf_write*, int, int, bool);
	using SuppressLists = bool(__thiscall*)(void*, int, bool);


    long __stdcall hkEndScene ( IDirect3DDevice9* device );
    long __stdcall hkReset ( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters );
    void __stdcall hkCreateMove ( int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket );
    void __stdcall hkCreateMove_Proxy ( int sequence_number, float input_sample_frametime, bool active );
    void __stdcall hkPaintTraverse ( vgui::VPANEL panel, bool forceRepaint, bool allowForce );
    void __stdcall hkEmitSound1 ( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk );
    //void __stdcall hkPlaySound(const char* name);
    void __stdcall hkDrawModelExecute ( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld );
    void __stdcall hkFrameStageNotify ( ClientFrameStage_t stage );
    void __stdcall hkOverrideView ( CViewSetup* vsView );
    void __stdcall hkLockCursor();
    int  __stdcall hkDoPostScreenEffects ( int a1 );
    bool __fastcall hkSvCheatsGetBool ( PVOID pConVar, void* edx );


	void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets* thisptr, DataUpdateType_t updateType);
	void __stdcall hkTEFireBulletsPostDataUpdate(DataUpdateType_t updateType);
	void __stdcall hkSuppressLists(int a2, bool a3);
    void __fastcall hkSceneEnd ( void* pEcx, void* pEdx );
    bool __stdcall hkFireEvent ( IGameEvent* pEvent );
	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output);
    void __stdcall Hooked_RenderSmokeOverlay ( bool unk );
    int __fastcall SendDatagram_h ( INetChannel* netchan, void*, bf_write* datagram );
	bool __fastcall hkWriteUsercmdDeltaToBuffer(IBaseClientDLL* ECX, void* EDX, int nSlot, bf_write* buf, int from, int to, bool isNewCmd);
    //void __fastcall DoExtraBonesProcessing(void *ecx, void *edx, studiohdr_t *hdr, Vector *vector, Quaternion *something, matrix3x4_t *matrix, CBoneBitList &bone_list, cik *context);
}
