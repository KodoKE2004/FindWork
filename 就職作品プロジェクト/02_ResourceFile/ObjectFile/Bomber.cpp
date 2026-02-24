#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>


namespace
{
    const NVector3 kDefaultPos   = NVector3(-  10.0f, -285.0f, 0.0f);
    const NVector3 kDefaultScale = NVector3( 1100.0f,  100.0f, 1.0f);

    const float kDefaultRopeU    = 0.045f;
    const float kOneBeatFillRate = 0.0311f; // 1”‚ÅFillRatio‚ª0.311Œ¸‚éi=4”‚Å0‚É‚È‚éj‚æ‚¤‚É‚·‚é
}

Bomber::Bomber(Camera& cam) : Square(cam)
{
}

void Bomber::Initialize()
{
    auto& instance = Game::GetInstance();
    auto& scene    = *instance.GetCurrentScene();
    TextureManager* textureMgr = instance;

    Square::Initialize();
    SetTexture(textureMgr->GetTexture("Bomber/Body.png"));
    SetPos   (- 560.0f,- 296.0f,  0.0f);
    SetRotate(  0.0f  ,    0.0f,- 0.1f);
    SetScale (  100.0f,  100.0f,  1.0f);
    SetShader("VS_Alpha", "PS_Alpha");

    m_Count     = 3;
    m_BaseRopeU = 1.0f;

    m_Rope = std::make_shared<Square>(scene.GetCamera());
    m_Rope->Initialize();
    m_Rope->SetTexture(textureMgr->GetTexture("Bomber/Rope.png"));
    m_Rope->SetName("m_Rope");

    m_Number = std::make_shared<Square>(scene.GetCamera());
    m_Number->Initialize();
    m_Number->SetTexture(textureMgr->GetTexture("Bomber/CountThree.png"));
    m_Number->SetPos(1000.0f,0.0f,0.0f);
    m_Number->SetName("m_Number");


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
    auto scene = Game::GetInstance().GetCurrentScene();
    
    if (m_Rope) {
        scene->DeleteObject(m_Rope);
    }
    
    if (m_Number) {
        scene->DeleteObject(m_Number);
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
    if(std::abs(m_FillRatio - clamped) < 0.001f)
    {
        return;
    }

    m_FillRatio = clamped;
    
    UpdateUV();
    ApplyFillTransform();
}

void Bomber::AdjustScaleByBeatTotal(int beatTotal, int baseBeat)
{
    if (!m_HasBase || baseBeat <= 0) {
        return;
    }

    const float clampedBeat = std::clamp(static_cast<float>(beatTotal), 0.0f, static_cast<float>(baseBeat));
    m_BaseRopeU = kDefaultRopeU + (kOneBeatFillRate * clampedBeat);
    
    m_BaseScale = kDefaultScale;
    m_BasePos   = kDefaultPos;
    m_BaseLeftX = m_BasePos.x - (m_BaseScale.x * 0.5f);

    m_FillRatio = 1.0f;
    UpdateUV();
    ApplyFillTransform();
}

void Bomber::UpdateUV()
{
    constexpr float minRatio = 0.001f;
    float width  = max(m_BaseRopeU, minRatio);
    float splitX = m_BaseRopeU / width;

    m_Rope->SetUV(splitX , 1.0f, 1.0f, 1.0f);
}

void Bomber::ApplyFillTransform()
{

    if (!m_Rope || !m_HasBase) {
        return;
    }

    const float clampedFill = std::clamp(m_FillRatio, 0.0f, 1.0f);
    const float removedU = m_BaseRopeU * (1.0f - clampedFill);
    const float currentU = m_BaseRopeU - removedU;

    NVector3 newScale = m_BaseScale;
    newScale.x = m_BaseScale.x * currentU;
    m_Rope->SetScale(newScale);

    const float newHalfW = newScale.x * 0.5f;
    const float left = m_BaseLeftX;

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

