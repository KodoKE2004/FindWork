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
    // ImGui::StyleColorsLight();

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

void DebugUI::Render(ID3D11ShaderResourceView* gameSrv, const ImVec2& gameSize) 
{
#ifdef _DEBUG
    s_HasGameViewRect = false;
    s_GameRenderSize = gameSize;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (gameSrv)
    {
        ImVec2 displaySize(gameSize.x * 0.8f, gameSize.y * 0.8f);
        ImGui::SetNextWindowPos(ImVec2(SCREEN_CORRECT_VALUE, SCREEN_CORRECT_VALUE), ImGuiCond_Always);
        ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("GameView",nullptr,flags);
        
        ImGui::Image((ImTextureID)gameSrv, displaySize);
        const ImVec2 winPos = ImGui::GetWindowPos();
        const ImVec2 crMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 crMax = ImGui::GetWindowContentRegionMax();
        s_GameViewMin = ImVec2(winPos.x + crMin.x, winPos.y + crMin.y);
        s_GameViewMax = ImVec2(winPos.x + crMax.x, winPos.y + crMax.y);
        s_HasGameViewRect = true;

        ImGui::End();
    }

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
    
    for (auto& f : m_debugfunction)
    {
        f();
    }
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // _DEBUG
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

void DebugUI::RedistDebugFunction(std::function<void(void)> f)
{
#ifdef _DEBUG

    m_debugfunction.push_back(std::move(f));

#endif // _DEBUG
}

