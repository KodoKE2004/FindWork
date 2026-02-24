#include "Bomber.h"
#include "Game.h"
#include <algorithm>
#include <cmath>


namespace
{
    const NVector3 kDefaultPos   = NVector3(-  10.0f, -285.0f, 0.0f);
    const NVector3 kDefaultScale = NVector3( 1100.0f,  100.0f, 1.0f);

    static constexpr float kDefaultRopeU    = 0.045f;
    static constexpr float kOneBeatFillRate = 0.0311f; // 1拍でFillRatioが0.311減る（=4拍で0になる）ようにする
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


    m_BasePos = kDefaultPos;
    m_BaseScale = kDefaultScale;
    m_BaseLeftX = m_BasePos.x - (m_BaseScale.x * 0.5f);

    m_BaseRopeU = 1.0f;   // baseRatio
    m_FillRatio = 1.0f;   // fillRatio
    m_HasBase = true;
    
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
    if (std::abs(m_FillRatio - clamped) < 0.001f) return;

    m_FillRatio = clamped;
    ApplyFillTransform();
}

void Bomber::AdjustScaleByBeatTotal(int beatTotal, int baseBeat)
{
    if (!m_Rope || baseBeat <= 0) return;

    // 5小節=1.0 とした時の「最大表示率」
    float baseRatio = std::clamp(static_cast<float>(beatTotal) / static_cast<float>(baseBeat),
        0.0f, 1.0f);

    m_BaseRopeU = baseRatio;

    // ベースは固定（5小節用のpos/scaleが基準）
    m_BasePos = kDefaultPos;
    m_BaseScale = kDefaultScale;
    m_BaseLeftX = m_BasePos.x - (m_BaseScale.x * 0.5f);
    m_HasBase = true;

    ApplyFillTransform();
}

void Bomber::ApplyFillTransform()
{
    if (!m_Rope || !m_HasBase) return;

    float baseRatio = std::clamp(m_BaseRopeU, 0.0f, 1.0f);
    float fill = std::clamp(m_FillRatio, 0.0f, 1.0f);

    // ★最終的に見せたい表示率（これ一本）
    float visibleRatio = baseRatio * fill;

    // UV：左固定で右を切る（SetUVの引数順はあなたのSquareに合わせて調整）
    // 典型: SetUV(u0, v0, u1, v1)
    m_Rope->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
    m_Rope->SetUVRect(0.0f, 0.0f, visibleRatio, 1.0f);

     // -----------------------------
     //   Scale（横幅だけ visibleRatio）
     // -----------------------------
    NVector3 s = m_BaseScale;
    s.x = m_BaseScale.x * visibleRatio;
    // 太さも「実ロープ部分だけ」にしたいならこれもアリ（任意）
    // s.y = m_BaseScale.y * (kRopeV1 - kRopeV0);
    m_Rope->SetScale(s);

    // -----------------------------
    //   Pos（左端固定。中心座標前提）
    // -----------------------------
    NVector3 p = m_BasePos;
    p.x = m_BaseLeftX + (s.x * 0.5f);
    m_Rope->SetPos(p);
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

