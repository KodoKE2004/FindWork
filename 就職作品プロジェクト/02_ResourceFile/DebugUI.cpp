#include "DebugUI.h"
#include "Game.h"

std::vector<std::function<void(void)>> DebugUI::m_debugfunction;
std::string DebugUI::TEXT_CurrentScene = "TitleScene" ;

namespace {
    ImVec2 s_GameViewMin{};
    ImVec2 s_GameViewMax{};
    ImVec2 s_GameRenderSize{};
    bool s_HasGameViewRect = false;
    float s_GameViewScale = 0.5f;
}

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

void DebugUI::Render(ID3D11ShaderResourceView* gameSrv, const ImVec2& gameSize) {
    s_HasGameViewRect = false;
    s_GameRenderSize = gameSize;
    // ImGui�̐V�����t���[�����J�n
    if (gameSrv)
    {
        ImGui::Begin("Game View");
        ImGui::SliderFloat("Scale", &s_GameViewScale, 0.1f, 1.0f, "%.2f");
        ImVec2 displaySize(gameSize.x * s_GameViewScale, gameSize.y * s_GameViewScale);
        ImGui::Image((ImTextureID)gameSrv, displaySize);
        s_GameViewMin = ImGui::GetItemRectMin();
        s_GameViewMax = ImGui::GetItemRectMax();
        s_HasGameViewRect = true;
        ImGui::End();
    }

    // �E�B���h�E�ƃf�o�b�O���̕`��

    // �f�o�b�O�֐��̎��s

    // �t���[���̃����_�����O������
}

bool DebugUI::GetGameViewRect(ImVec2& outMin, ImVec2& outMax)
{
#ifdef _DEBUG
    if (!s_HasGameViewRect)
    {
        return false;
    }
    outMin = s_GameViewMin;
    outMax = s_GameViewMax;
    return true;
#else
    (void)outMin;
    (void)outMax;
    return false;
#endif
}

ImVec2 DebugUI::GetGameRenderSize()
{
    return s_GameRenderSize;
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
