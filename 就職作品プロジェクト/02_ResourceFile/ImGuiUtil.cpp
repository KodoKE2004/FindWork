#include "ImGuiUtil.h"
#include "DebugUI.h"

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
        if (r.size.x > 0.0f && r.size.y > 0.0f)
        {
            r.normalized = ImVec2(r.local.x / r.size.x, r.local.y / r.size.y);
            r.leftHandedNormalized = ImVec2(
                r.normalized.x - 0.5f,
                0.5f - r.normalized.y
            );
        }
    }

    return r;
}

GuiMousePos GetMousePosInGameView()
{
    GuiMousePos r{};
    ImVec2 viewMin{};
    ImVec2 viewMax{};
    if (!DebugUI::GetGameViewRect(viewMin, viewMax))
    {
        return r;
    }

    const ImVec2 mouse = ImGui::GetMousePos();
    r.size = ImVec2(viewMax.x - viewMin.x, viewMax.y - viewMin.y);

    r.inside =
        mouse.x >= viewMin.x && mouse.x < viewMax.x &&
        mouse.y >= viewMin.y && mouse.y < viewMax.y;

    if (r.inside)
    {
        // GuiÀ•W‚ð”÷’²®‚Å‰ÁŽZ‚³‚ê‚½’l•ª‰ÁŽZ
        r.local = ImVec2(
            (mouse.x + SCREEN_CORRECT_VALUE) - viewMin.x,
            (mouse.y + SCREEN_CORRECT_VALUE) - viewMin.y
        );
        if (r.size.x > 0.0f && r.size.y > 0.0f)
        {
            r.normalized = ImVec2(r.local.x / r.size.x, r.local.y / r.size.y);
            r.leftHandedNormalized = ImVec2(
                r.normalized.x - 0.5f,
                0.5f - r.normalized.y
            );
        }
    }

    return r;
}