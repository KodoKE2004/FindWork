#include "Theme.h"
#include "Game.h"
#include "Application.h"
#include <algorithm>

using namespace Math::Easing;

Theme::Theme(Camera& cam):Square(cam)
{
}

void Theme::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureManager = instance;

    Square::Initialize();

    m_Duration  = 2.0f;
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

    float t = std::clamp(m_Elapsed / max(m_Duration, 0.0001f), 0.0f, 1.0f);
    float ease = EvaluateEasing(EASING_TYPE::IN_EXPO, t);
    float mass = max(0.6f, m_ScaleMass - (m_ScaleMass * ease));

    scale *= mass; 
    
    SetScale(scale);
}

void Theme::Draw()
{
    if(!m_isActive){ return; }
    Square::Draw();
    // Debug::Log("[[描画]] Themeオブジェクト");

}

void Theme::Finalize()
{
    Square::Finalize();
}

void Theme::SetActive()
{   
    m_isActive = true;
    m_Elapsed = 0.0f;
}

void Theme::SetActive(const bool isActive)
{
    m_isActive = isActive;
    if (m_isActive)
    {
        m_Elapsed = 0.0f;
    }
}

void Theme::SetScaleBase(NVector3 scale)
{
    m_ScaleBase = scale; 
}
