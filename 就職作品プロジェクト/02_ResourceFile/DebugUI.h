#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <functional>
#include "Application.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class DebugUI {
    static std::vector<std::function<void(void)>> m_debugfunction;
public:
    static std::string TEXT_CurrentScene;
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context);

    static void Render(ID3D11ShaderResourceView* gameSrv, const ImVec2& gameSize);
    static bool GetGameViewRect(ImVec2& outMin, ImVec2& outMax);
    static ImVec2 GetGameRenderSize();
    static void RedistDebugFunction(std::function<void(void)> f);

    static void Render();

    static void DisposeUI();
};

