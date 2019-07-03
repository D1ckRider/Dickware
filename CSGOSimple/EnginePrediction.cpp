
#include "EnginePrediction.h"
#include "RuntimeSaver.h"
#include "helpers\math.hpp"

void EnginePrediction::run_prediction(CUserCmd* cmd, C_BasePlayer* player)
{
   /* static int flTickBase;
    static CUserCmd* pLastCmd;
    auto local_player = g_LocalPlayer;
    if (!local_player)
    {
        return;
    }

    // fix tickbase if game didnt render previous tick
    if (pLastCmd)
    {
        if (pLastCmd->hasbeenpredicted)
        {
            flTickBase = local_player->m_nTickBase();
        }
        else
        {
            ++flTickBase;
        }
    }

    pLastCmd = cmd;
    flOldCurtime = g_GlobalVars->curtime;
    flOldFrametime = g_GlobalVars->frametime;
    //SDK::CGV::uRandomSeed = getRandomSeed();



    g_Saver.curtime = flTickBase * g_GlobalVars->interval_per_tick;
    //g_GlobalVars->curtime = flTickBase * g_GlobalVars->interval_per_tick;
    g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

    /*
    //INTERFACES::pMovement->StartTrackPredictionErrors(local_player);
    g_GameMovement->StartTrackPredictionErrors(local_player);

    //CMoveData

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    g_MoveHelper->SetHost(local_player);
    //g_pre
    g_Prediction->SetupMove(local_player, cmd, g_MoveHelper, &data);
    g_GameMovement->ProcessMovement(local_player, &data);
    g_Prediction->FinishMove(local_player, cmd, &data);
    */

	* predictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
	predictionPlayer = player;

	m_flOldCurTime = g_GlobalVars->curtime;
	m_flOldFrametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = player->m_nTickBase() * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	//Here we're doing CBasePlayer::UpdateButtonState // NOTE: hard to tell when offsets changed, think of more longterm solution or just dont do this.
	moveData.m_nButtons = cmd->buttons;
	int buttonsChanged = cmd->buttons ^ *reinterpret_cast<int*>(uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31DC) = (uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E8) = cmd->buttons;
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E0) = cmd->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E4) = buttonsChanged & ~cmd->buttons; //m_afButtonReleased ~ The ones not down are "released"

	g_GameMovement->StartTrackPredictionErrors(player);

	memset(&moveData, 0, sizeof(CMoveData));
	g_MoveHelper->SetHost(player);
	g_Prediction->SetupMove(player, cmd, g_MoveHelper, &moveData);
	g_GameMovement->ProcessMovement(player, &moveData);
	g_Prediction->FinishMove(player, cmd, &moveData);
}

void EnginePrediction::end_prediction(C_BasePlayer* player)
{
	g_GameMovement->FinishTrackPredictionErrors(player);
	g_MoveHelper->SetHost(nullptr);

	*predictionRandomSeed = -1;
	predictionPlayer = nullptr;

	g_GlobalVars->curtime = m_flOldCurTime;
	g_GlobalVars->frametime = m_flOldFrametime;
    /*auto local_player = g_LocalPlayer;
    if (!local_player)
    {
        return;
    }

    //g_GameMovement->FinishTrackPredictionErrors(local_player);
    //g_MoveHelper->SetHost(nullptr);

    g_GlobalVars->curtime = flOldCurtime;*/
    //if (!g_Saver.InFakewalk) g_GlobalVars->frametime = flOldFrametime;
}

/*
 * setup
 * Sets up prediction so that we can perfrom full game prediction
 */
void EnginePrediction::Setup(player_prediction_data& data)
{
	if (data.prediction_stage != 0 || !g_LocalPlayer)
		return;

	static auto random_seed_sig = *reinterpret_cast<unsigned**>(Utils::PatternScan(GetModuleHandle(L"client_panorama.dll"), "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);

	data.curtime = g_GlobalVars->curtime;
	data.frametime = g_GlobalVars->frametime;

	if (random_seed_sig != nullptr)
	{
		data.prediction_random_seed = random_seed_sig;
		data.random_seed = *random_seed_sig;
	}

	g_GlobalVars->curtime = static_cast<float>(g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick);
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	data.non_predicted.flags = g_LocalPlayer->m_fFlags();
	data.non_predicted.curtime = g_GlobalVars->curtime;

	data.prediction_stage = 1;
}

/*
 * predict_engine
 * Predicts the game 1 tick forwards
 */
void EnginePrediction::RunPrediction(player_prediction_data& data, CUserCmd* cmd)
{
	if (data.prediction_stage != 1 || !g_LocalPlayer)
		return;

	data.in_prediction = g_Prediction->in_prediction;
	data.tickbase = g_LocalPlayer->m_nTickBase();

	if (data.prediction_random_seed != nullptr)
		*data.prediction_random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF; 

	g_Prediction->in_prediction = true;

	if (g_Prediction->engine_paused)
		g_GlobalVars->frametime = 0;

	CMoveData move_data;
	memset(&move_data, 0, sizeof(move_data));

	//static auto sv_footsteps = G::interfaces.cvar->find_var("sv_footsteps");
	//auto sv_footsteps_old = sv_footsteps->get_float();

	//sv_footsteps->change_callbacks = nullptr;
	//sv_footsteps->set_value(0.f);
	g_MoveHelper->SetHost(g_LocalPlayer);
	g_Prediction->SetupMove(g_LocalPlayer, cmd, g_MoveHelper, &move_data);
	g_GameMovement->ProcessMovement(g_LocalPlayer, &move_data);
	g_LocalPlayer->m_nTickBase() = data.tickbase;
	g_Prediction->FinishMove(g_LocalPlayer, cmd, &move_data);
	//sv_footsteps->set_value(sv_footsteps_old);

	g_Prediction->in_prediction = data.in_prediction;

	data.prediction_stage = 2;
}


/*
 * finish
 * Finishes prediction and restores all values
 */

void EnginePrediction::EndPrediction(player_prediction_data& data)
{
	if (data.prediction_stage != 2 || !g_LocalPlayer)
		return;

	g_MoveHelper->SetHost(nullptr);

	if (data.prediction_random_seed != nullptr)
		*data.prediction_random_seed = data.random_seed;

	g_GlobalVars->curtime = data.curtime;
	g_GlobalVars->frametime = data.frametime;
}

EnginePrediction* prediction = new EnginePrediction();