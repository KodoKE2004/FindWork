#include "Theme.h"
#include "../Audio.h"
#include "Game.h"
#include "Application.h"
#include "Renderer.h"
#include "Debug.hpp"
#include <algorithm>

using namespace Math::Easing;

Theme::Theme(Camera& cam):Square(cam)
{
}

void Theme::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureManager = instance;
    RhythmBeat      rhythmBeat = instance.GetCurrentScene()->GetRelationData().rhythmBeat;
    Square::Initialize();
    
    m_SecondPerBeat = rhythmBeat.GetBeatConst().secondsPerBeat;
    m_Duration  = 1.5f * m_SecondPerBeat;
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
    
    // 一定の拍が経ったらactiveをfalseにする
    float activeBeat = 5.0f * m_SecondPerBeat;
    if (activeBeat < m_Elapsed) 
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
