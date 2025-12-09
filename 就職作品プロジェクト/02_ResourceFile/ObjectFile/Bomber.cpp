#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>

Bomber::Bomber(Camera* cam) : Square(cam)
{
}

void Bomber::Initialize()
{
    Square::Initialize();


    TextureManager* textureMgr = Game::GetInstance();
    SetTexture(textureMgr->GetTexture("Bomber.png"));

    const NVector3 basePos   = NVector3(-275.0f, -285.0f, 0.0f);
    const NVector3 baseScale = NVector3( 700.0f,  100.0f, 1.0f);

    SetPos  (basePos);
    SetScale(baseScale);

    m_BasePos     = basePos;
    m_BaseScale   = baseScale;
    m_HasBase     = true;
    m_isReadyExpo = false;
    m_Count = 3;

    SetShader("VS_Alpha", "PS_Alpha");
    
    UpdateUV();
    ApplyFillTransform();
}

void Bomber::Update()
{
    
}

void Bomber::Draw()
{
    Square::Draw();
}

void Bomber::Finalize()
{
    Square::Finalize();
}

void Bomber::CountDown()
{
    if (m_Count < 0) {
        return;
    }

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    


    switch (m_Count)
    {
    case 3: 
        SetPos  (-575.0f, GetPos().y,GetPos().z);
        SetScale(100.0f,100.0f,1.0f);
        SetUV(1.0f, 1.0f, 1.0f, 1.0f);
        SetTexture(textureMgr->GetTexture("BomberCountThree.png")); 
    break;
    case 2: SetTexture(textureMgr->GetTexture("BomberCountTwo.png"))  ; break;
    case 1: SetTexture(textureMgr->GetTexture("BomberCountOne.png"))  ; break;
    case 0: 
        SetScale(150.0f,150.0f,1.0f);
        SetTexture(textureMgr->GetTexture("BomberExplosion.png")) ; 
    break;
    }
    --m_Count;
}

void Bomber::SetFillRatio(float ratio)
{
    float clamped = std::clamp(ratio, 0.0f, 1.0f);
    if(std::abs(m_FillRatio - clamped) < 0.001f)
    {
        return;
    }

    m_FillRatio = clamped;
    
    UpdateUV();
    ApplyFillTransform();
}

void Bomber::UpdateUV()
{
    constexpr float minRatio = 0.01f;
    float width = max(m_FillRatio, minRatio);
    float splitX = 1.0f / width;

    SetUV(1.0f,1.0f, splitX, 1.0f);
}

void Bomber::ApplyFillTransform()
{
    if (!m_HasBase) {
        return;
    }

    constexpr float minRatio = 0.12f;
    float widthRatio = max(m_FillRatio, minRatio);

    NVector3 newScale = m_BaseScale;
    newScale.x = m_BaseScale.x * widthRatio;
    SetScale(newScale);

    float baseHalfW = m_BaseScale.x * 0.5f;
    float newHalfW = newScale.x * 0.5f;

    float left = m_BasePos.x - baseHalfW;

    NVector3 newPos = m_BasePos;
    newPos.x = left + newHalfW;

    SetPos(newPos);

}

