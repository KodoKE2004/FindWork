#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>


namespace
{
    const NVector3 kDefaultPos   = NVector3(-  10.0f, -285.0f, 0.0f);
    const NVector3 kDefaultScale = NVector3( 1100.0f,  100.0f, 1.0f);

    // 残り一小節のロープの長さの比率
    constexpr float kDefaultRopeU    = 0.045f;
    constexpr float kOneBeatFillRate = 0.0311f; // 1拍でFillRatioが0.311減る
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

    // Objectの左端を基準にTextureのUVに合わせてPosとScaleを補正
    m_BaseLeftX = m_BasePos.x - (m_BaseScale.x * 0.5f);

   float _BeatTotal = static_cast<float>(beatTotal);
    // UVの全体比率の調整
    m_OneBeatFillRate = kDefaultRopeU + (kOneBeatFillRate * _BeatTotal);
    m_OneBeatFillRate = max(m_OneBeatFillRate, 0.001f);

    // beatTotalに応じてRopeの最大の長さを調整
    const float scaleRate = m_OneBeatFillRate / 1.0f;
    m_BaseScale   = kDefaultScale;
    m_BaseScale.x = kDefaultScale.x * scaleRate;

    ApplyFillTransform();
}

void Bomber::UpdateUV()
{
    constexpr float minRatio = 0.001f;
    float width = max(m_OneBeatFillRate, minRatio);
    float splitX = m_BaseRopeU / width;

    m_Rope->SetUV(m_BaseRopeU, 1.0f, splitX, 1.0f);
}

void Bomber::ApplyFillTransform()
{
    if (!m_Rope || !m_HasBase) {
        return;
    }

    constexpr float minRatio = 0.001f;
    float widthRatio = max(m_OneBeatFillRate, minRatio);

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

