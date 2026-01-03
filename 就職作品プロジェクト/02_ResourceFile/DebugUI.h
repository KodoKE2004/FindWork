#pragma once

#include "Application.h"
#include <d3d11.h>
#include <functional>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class DebugUI {
    static std::vector<std::function<void(void)>> m_debugfunction;
public:
    static std::string TEXT_CurrentScene;
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context);

    // デバッグ表示関数の登録
    static void RedistDebugFunction(std::function<void(void)> f);

    static void Render();

    static void DisposeUI();
};

