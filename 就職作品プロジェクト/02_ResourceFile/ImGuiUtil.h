#pragma once
#include <imgui.h>

struct GuiMousePos
{
    bool   inside = false;
    ImVec2 local{};
    ImVec2 size{};
    ImVec2 normalized{};
    ImVec2 leftHandedNormalized{};
};

GuiMousePos GetMousePosInCurrentGui();
GuiMousePos GetMousePosInGameView();