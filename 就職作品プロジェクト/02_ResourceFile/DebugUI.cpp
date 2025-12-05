#include "DebugUI.h"
#include "Game.h"

std::vector<std::function<void(void)>> DebugUI::m_debugfunction;
std::string DebugUI::TEXT_CurrentScene = "TitleScene" ;

void DebugUI::Init(ID3D11Device* device, ID3D11DeviceContext* context) 
{
#ifdef _DEBUG
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Application::GetWindow());
    ImGui_ImplDX11_Init(device, context);
#endif 
}

void DebugUI::DisposeUI() {
#ifdef _DEBUG

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

#endif // _DEBUG
}

// デバッグ表示関数の登録
void DebugUI::RedistDebugFunction(std::function<void(void)> f) {
#ifdef _DEBUG

    m_debugfunction.push_back(std::move(f));

#endif // _DEBUG
}

void DebugUI::Render() {
#ifdef _DEBUG

    // ImGuiの新しいフレームを開始
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ウィンドウとデバッグ情報の描画
    const char* text = TEXT_CurrentScene.c_str();
    ImGui::Begin(text);
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    static bool reloadSuccess = true;
    static bool reloadAttempted = false;
    if (ImGui::Button("Hot Reload Shaders"))
    {
        ShaderManager* shaderMgr = Game::GetInstance();
        if (shaderMgr)
        {
            reloadSuccess = shaderMgr->ReloadAll();
            reloadAttempted = true;
        }
    }
    if (reloadAttempted)
    {
        ImGui::SameLine();
        ImGui::Text(reloadSuccess ? "Reload succeeded" : "Reload failed");
    }

    ImGui::End();

    // デバッグ関数の実行
    for (auto& f : m_debugfunction)
    {
        f();
    }

    // フレームのレンダリングを完了
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // _DEBUG
}
