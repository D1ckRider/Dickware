#pragma once

#include <string>
#include "valve_sdk\misc\Color.hpp"
#include "singleton.hpp"
#include "imgui/imgui.h"

struct IDirect3DDevice9;

class MenuHelper : public Singleton<MenuHelper>
{
public:
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Render();

    void Show();
    void Hide();
    void Toggle();

    bool IsVisible() const
    {
        return _visible;
    }
    int get_fps();

    /* menu components */
    class components
    {
    public:
        bool StartWindow(std::string title, ImVec2 size, ImGuiWindowFlags flags);
        void EndWindow();
        void NavbarIcons(char* items[], char* hovered_items[], int size, int& selected, ImFont* font);
        void Navbar(char* items[], int size, int& selected);
        void HelpMarker(const char* text);
		void Checkbox(std::string label, bool& value);
		void ColorCheckbox(std::string text, bool &name, Color &color);
		void ColorCheckbox2(std::string text, bool &name, Color &color1, Color &color2);
		void Label(std::string text);
		void ComboBox(std::string text, const char* items[], int size, int& value);
		void ComboCheckBox(std::string label, const char* items[], bool* values[], int size);
		void ComboFilterBox(std::string label, const char* items[], int size, int& value);
        void BeginChild(std::string id, ImVec2 size, bool border = true, ImGuiWindowFlags flags = 0);
        void EndChild();
        void Columns(int count, bool border, char* id = NULL);
        void NextColumn();
        void SameLine();
		void SliderInt(std::string label, int& value, int min, int max);
		void SliderFloat(std::string label, float& value, float min, float max);
		void Hotkey(std::string label, int& value, const int& type = 0);
        bool Button(std::string text);
        void Spacing();
		bool MessageDialog(std::string header, std::string msg);

    private:
        bool InColumns = false;
        int ColumnSize = 0;
        int ColumnCurrent = 0;
        bool FrameStarted = false;
        ImGuiStyle* style;
        float x = 0.f;
        float y = 0.f;
        int CurrentItem = 0;
        bool NavbarHovered[128][128];
    };

    void CreateStyle();
    ImGuiStyle        _style;
    bool              _visible;
};