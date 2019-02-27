
#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "Settings.h"
#include "hooks.hpp"
#include "menu_helpers.hpp"
#include "RuntimeSaver.h"
#include "options.hpp"
#include "render.hpp"
#include "EventHelper.h"
#include "ConsoleHelper.h"
#include "Logger.h"
#include "features\Skinchanger.h"
#include <filesystem>

namespace fs = std::filesystem;

DWORD WINAPI OnDllAttach ( LPVOID base )
{
    //
    // Wait at most 10s for the main game modules to be loaded.
    //
    if ( Utils::WaitForModules ( 10000, { L"client_panorama.dll", L"engine.dll", L"shaderapidx9.dll" } ) == WAIT_TIMEOUT )
    {
        // One or more modules were not loaded in time
        return FALSE;
    }

    #ifdef _DEBUG
    Utils::AttachConsole();
    #endif

    try
    {
        Utils::ConsolePrint ( "Initializing...\n" );

        Interfaces::Initialize();
        Interfaces::Dump();

        NetvarSys::Get().Initialize();
        InputSys::Get().Initialize();
        Render::Get().Initialize();
        MenuHelper::Get().Initialize();

		Settings::Initialize();
//		Skinchanger::Get().InitalizeSkins();
        Hooks::Initialize();

        // Register some hotkeys.
        // - Note:  The function that is called when the hotkey is pressed
        //          is called from the WndProc thread, not this thread.
        //

        // Panic button
        #ifdef _DEBUG
        InputSys::Get().RegisterHotkey ( VK_NUMPAD0, [base]()
        {
            g_Unload = true;
        } );
        #endif // _DEBUG

        // Menu Toggle
        InputSys::Get().RegisterHotkey ( VK_INSERT, [base]()
        {
            MenuHelper::Get().Toggle();
        } );

        EventHelper::Get().init();

        Utils::ConsolePrint ( "Finished.\n" );

        //g_Config.AppdataFolder
        //fs::
        //fs::get

        while ( !g_Unload )
            Sleep ( 250 );

        g_Logger.Info ( "cheat", "unloading cheat" );

        Sleep ( 500 );

        g_CVar->FindVar ( "crosshair" )->SetValue ( true );

        EventHelper::Get().unInit();

        FreeLibraryAndExitThread ( static_cast<HMODULE> ( base ), 1 );

    }
    catch ( const std::exception& ex )
    {
        #ifdef _DEBUG
        Utils::ConsolePrint ( ex.what() );
        Utils::ConsolePrint ( "An error occured during initialization:\n" );
        Utils::ConsolePrint ( "%s\n", ex.what() );
        Utils::ConsolePrint ( "Press any key to exit.\n" );
        Utils::ConsoleReadKey();
        Utils::DetachConsole();
        #endif // _DEBUG

        FreeLibraryAndExitThread ( static_cast<HMODULE> ( base ), 1 );
    }

    // unreachable
    //return TRUE;
}

BOOL WINAPI OnDllDetach()
{
    #ifdef _DEBUG
    Utils::DetachConsole();
    #endif

    Hooks::Shutdown();

    MenuHelper::Get().Shutdown();
    return TRUE;
}

BOOL WINAPI DllMain (
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch ( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls ( hinstDll );
            CreateThread ( nullptr, 0, OnDllAttach, hinstDll, 0, nullptr );
			g_Saver.csgo = HMODULE(hinstDll);
            return TRUE;

        case DLL_PROCESS_DETACH:
            if ( lpvReserved == nullptr )
                return OnDllDetach();

            return TRUE;

        default:
            return TRUE;
    }
}
