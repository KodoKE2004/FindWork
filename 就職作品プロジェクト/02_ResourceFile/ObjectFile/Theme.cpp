#include "Theme.h"
#include "../Audio.h"
#include "Game.h"
#include "Application.h"
#include "Renderer.h"
#include "Debug.hpp"
#include <algorithm>

using namespace Calculator::Easing;

Theme::Theme(Camera& cam):Square(cam)
{
}

void Theme::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureManager = instance;
    Square::Initialize();
    
    const GameplaySession& session = Game::GetGameplaySession();
    m_SecondPerBeat = session.GetRhythmBeat().GetOneBeat();
    m_Duration  = m_SecondPerBeat;
    m_ScaleMass = 5.0f;
    m_Elapsed   = 0.0f;

    SetShader("VS_Alpha","PS_Alpha");
    SetTexture(textureManager->GetTexture("Plane.png"));

}

void Theme::Update()
{
    if(!m_isActive){ 
        return; 
    }

    NVector3 scale = m_ScaleBase;
    m_Elapsed += Application::GetDeltaTime();
    
    // お題表示時間時間：5/4小節 ※現在8/8小節計算のため2倍
    const float activeBeat = 5.0f * 2.0f;

    // 一定の拍が経ったらactiveをfalseにする
    float activeTime = activeBeat * m_SecondPerBeat * 2.0f;

    if (activeTime < m_Elapsed) 
    {
        m_isActive = false;
        return;
    }

    float t = std::clamp(m_Elapsed / max(m_Duration, 0.0001f), 0.0f, 1.0f);
    float ease = EvaluateEasing(EASING_TYPE::IN_EXPO, t);
    float mass = max(1.0f, m_ScaleMass - (m_ScaleMass * ease));

    scale *= mass; 
    
    SetScale(scale);
}

void Theme::Draw()
{
    if(!m_isActive){ return; }
    // State破壊の影響を受けないよう、パイプラインを先頭で再設定する
    SetPipeline();

    static uint64_t s_LastLogFrame = 0;
    const auto frame = Game::GetDrawFrameCounter();
    if (frame != s_LastLogFrame) {
        s_LastLogFrame = frame;
    }

    Square::Draw();

}

void Theme::Finalize()
{
    Square::Finalize();
}

void Theme::SetActive()
{   
    m_isActive = true;
    m_Elapsed = 0.0f;
    Debug::Log("[[検出]] Theme Active");
}

void Theme::SetActive(const bool isActive)
{
    if (m_isActive) {
        return;
    }

    m_isActive = isActive;
    if (m_isActive)
    {
        m_Elapsed = 0.0f;
        Debug::Log("[[検出]] Theme Active");
    }
}

void Theme::SetScaleBase(NVector3 scale)
{
    m_ScaleBase = scale; 
}
