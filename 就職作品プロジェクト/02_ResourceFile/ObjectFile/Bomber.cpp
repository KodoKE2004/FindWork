#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>

Bomber::Bomber(Camera& cam) : Square(cam)
{
}

void Bomber::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    Square::Initialize();
    SetTexture(textureMgr->GetTexture("BomberBody.png"));
    SetPos   (-560.0f,-296.0f, 0.0f);
    SetRotate( 0.0f  ,   0.0f,-0.1f);
    SetScale ( 100.0f, 100.0f, 1.0f);
    SetShader("VS_Alpha", "PS_Alpha");

    m_Count = 3;


    m_Rope = instance.AddObject<Square>();
    m_Rope->SetTexture(textureMgr->GetTexture("BomberRope.png"));
    m_Rope->SetName("m_Rope");
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Rope);

    m_Number = instance.AddObject<Square>();
    m_Number->SetTexture(textureMgr->GetTexture("CountThree.png"));
    m_Number->SetPos(1000.0f,0.0f,0.0f);
    m_Number->SetName("m_Number");
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Number);

    const NVector3 basePos   = NVector3(-110.0f, -285.0f, 0.0f);
    const NVector3 baseScale = NVector3( 900.0f,  100.0f, 1.0f);

    m_Rope->SetPos  (basePos  );
    m_Rope->SetScale(baseScale);

    m_BasePos     = basePos;
    m_BaseScale   = baseScale;
    m_HasBase     = true;
    m_isReadyExpo = false;
    

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
    
   if (m_Count == 0)
   {
        SetTexture(textureMgr->GetTexture("BomberExplosion.png")); 
   }
   else 
   {
        CountDownTexture();
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
    if(!m_Rope) return;

    if (!m_Rope) {
        return;
    }

    constexpr float minRatio = 0.0f;
    float width = max(m_FillRatio, minRatio);
    if (width <= 0.0f)
    {
        m_Rope->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
        return;
    }
    float splitX = 1.0f / width;
}

void Bomber::ApplyFillTransform()
{

    if (!m_Rope || !m_HasBase) {
        return;
    }

    constexpr float minRatio = 0.0f;
    float widthRatio = max(m_FillRatio, minRatio);

    NVector3 newScale = m_BaseScale;
    newScale.x = m_BaseScale.x * widthRatio;
    m_Rope->SetScale(newScale);

    float baseHalfW = m_BaseScale.x * 0.5f;
    float newHalfW  = newScale.x    * 0.5f;

    float left = m_BasePos.x - baseHalfW;

    NVector3 newPos = m_BasePos;
    newPos.x = left + newHalfW;

    m_Rope->SetPos(newPos);

}

void Bomber::CountDownTexture()
{   
    if (m_Count > 3) {
        return;
    }

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    
    switch (m_Count)
    {
    case 3: m_Number->SetTexture(textureMgr->GetTexture("CountThree.png")); break;
    case 2: m_Number->SetTexture(textureMgr->GetTexture("CountTwo.png"));   break;
    case 1: m_Number->SetTexture(textureMgr->GetTexture("CountOne.png"));   break;
    default: m_Number->SetPos(1000.0f, 0.0f, 0.0f); break;
    }

}

