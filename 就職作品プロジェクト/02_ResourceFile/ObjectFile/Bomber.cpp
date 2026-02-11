#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>


namespace
{
    const NVector3 kDefaultPos   = NVector3(-110.0f, -285.0f, 0.0f);
    const NVector3 kDefaultScale = NVector3( 900.0f,  100.0f, 1.0f);
}

Bomber::Bomber(Camera& cam) : Square(cam)
{
}

void Bomber::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    Square::Initialize();
    SetTexture(textureMgr->GetTexture("Bomber/Body.png"));
    SetPos   (- 560.0f,- 296.0f,  0.0f);
    SetRotate(  0.0f  ,    0.0f,- 0.1f);
    SetScale (  100.0f,  100.0f,  1.0f);
    SetShader("VS_Alpha", "PS_Alpha");

    m_Count = 3;

    m_Rope = instance.AddObject<Square>();
    m_Rope->SetTexture(textureMgr->GetTexture("Bomber/Rope.png"));
    m_Rope->SetName("m_Rope");
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Rope);

    m_Number = instance.AddObject<Square>();
    m_Number->SetTexture(textureMgr->GetTexture("Bomber/CountThree.png"));
    m_Number->SetPos(1000.0f,0.0f,0.0f);
    m_Number->SetName("m_Number");
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Number);


    m_Rope->SetPos  (kDefaultPos  );
    m_Rope->SetScale(kDefaultScale);

    m_BasePos     = kDefaultPos;
    m_BaseScale   = kDefaultScale;
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
    auto& instance = Game::GetInstance();

    if (m_Rope) {
        instance.DeleteObject(m_Rope);
    }
    
    if (m_Number) {
        instance.DeleteObject(m_Number);
    }
    
    m_Rope.reset();
    m_Number.reset();
    
    Square::Finalize();
}

void Bomber::CountDown()
{
    if(m_Count < 0){
        return;
    }

    CountDownTexture();

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    if (m_Count == 0) {
        SetTexture(textureMgr->GetTexture("Bomber/Explosion.png")); 
    }

    --m_Count;
}

void Bomber::SetFillRatio(float ratio)
{
    float clamped = std::clamp(ratio, 0.0f, 1.0f);
    if(std::abs(1.0f - clamped) < 0.001f)
    {
        return;
    }

    m_FillRatio = clamped;
    
    UpdateUV();
    ApplyFillTransform();
}

void Bomber::AdjustScaleByBeatTotal(int beatTotal, int maxBeat)
{
    if (!m_HasBase || maxBeat <= 0) {
        return;
    }

    const float clampedBeat = std::clamp(static_cast<float>(beatTotal), 0.0f, static_cast<float>(maxBeat));
    const float scaleRate = clampedBeat / static_cast<float>(maxBeat);

    m_BaseScale   = kDefaultScale;
    m_BaseScale.x = kDefaultScale.x * scaleRate;

    m_BasePos   = kDefaultPos;
    m_BasePos.x = m_BaseLeftX + (m_BaseScale.x * 0.5f);
    ApplyFillTransform();
}

void Bomber::UpdateUV()
{
    constexpr float minRatio = 0.0f;
    float width = max(m_FillRatio, minRatio);
    float splitX = 1.0f / width;

    m_Rope->SetUV(1.0f, 1.0f, splitX, 1.0f);
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
    case 3: m_Number->SetTexture(textureMgr->GetTexture("Bomber/CountThree.png")); 
            m_Number->SetPos(- 560.0f, - 230.0f, 0.0f);
    break;
    case 2: m_Number->SetTexture(textureMgr->GetTexture("Bomber/CountTwo.png"));
            m_Number->SetPos(- 560.0f, - 230.0f, 0.0f);
    break;
    case 1: m_Number->SetTexture(textureMgr->GetTexture("Bomber/CountOne.png")); 
            m_Number->SetPos(- 560.0f, - 230.0f, 0.0f);    
    break;
    default: m_Number->SetPos(1000.0f, 0.0f, 0.0f); break;
    }

}

