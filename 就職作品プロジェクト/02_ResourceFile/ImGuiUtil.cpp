#include "ImGuiUtil.h"

GuiMousePos GetMousePosInCurrentGui()
{
    GuiMousePos r{};

    const ImVec2 winPos = ImGui::GetWindowPos();
    const ImVec2 crMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 crMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 mouse = ImGui::GetMousePos();

    const ImVec2 contentTL = ImVec2(winPos.x + crMin.x, winPos.y + crMin.y);
    const ImVec2 contentBR = ImVec2(winPos.x + crMax.x, winPos.y + crMax.y);

    r.size = ImVec2(
        contentBR.x - contentTL.x,
        contentBR.y - contentTL.y
    );

    r.inside =
        mouse.x >= contentTL.x && mouse.x < contentBR.x &&
        mouse.y >= contentTL.y && mouse.y < contentBR.y;

    if (r.inside)
    {
        r.local = ImVec2(
            mouse.x - contentTL.x,
            mouse.y - contentTL.y
        );
    }

    return r;
}
