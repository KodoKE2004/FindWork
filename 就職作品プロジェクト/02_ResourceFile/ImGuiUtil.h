#pragma once
#include <imgui.h>

struct GuiMousePos
{
    bool   inside = false;
    ImVec2 local{};
    ImVec2 size{};
};

GuiMousePos GetMousePosInCurrentGui();