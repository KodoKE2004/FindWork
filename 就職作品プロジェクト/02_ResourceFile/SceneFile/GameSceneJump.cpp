#include "GameSceneJump.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <random>
#include "GameSceneText.h"

float GameSceneJump::GenerateActivationDelay()
{

    static std::mt19937 engine{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(1.0f, 2.0f);

    return dist(engine);
}

void GameSceneJump::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneJump";

    // 基底クラスの初期化
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_JUMP;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    // カートの生成
    m_Cart = instance.AddObject<Cart>(instance.GetCamera());
    m_Cart->SetTexture(textureMgr->GetTexture("Car.png"));
    m_Cart->CreateStartPattern(0);              // パターンを登録
    m_Cart->SetSpeedFactor(0.0f);
    m_Cart->SetStartPattern();                  // カートの開始パターン設定

    m_MySceneObjects.emplace_back(m_Cart);

    m_TimeCartActivetion.limit = GenerateActivationDelay();

    // カート警告の生成
    m_CartWarning = instance.AddObject<CartWarning>(instance.GetCamera());
    m_CartWarning->SetTexture(textureMgr->GetTexture("CartWarning.png"));
    m_CartWarning->SetColor(1.0f, 0.25f, 0.25f, 0.85f);
    m_CartWarning->SetBaseScale(NVector3(200.0f, 200.0f, 1.0f));
    m_CartWarning->SetTowardsCenterOffset(200.0f);
    m_CartWarning->SetAdditionalOffset(NVector3(0.0f, 80.0f, 0.0f));
    m_CartWarning->Deactivate();
    m_MySceneObjects.emplace_back(m_CartWarning);

    SetTimer(&m_TimeCartActivetion.timer);
    SetTimer(&m_TimeCartWarning.timer);
    SetTimer(&m_Cart->m_MoveTimer.x);
    SetTimer(&m_Cart->m_MoveTimer.y);

    m_HasSpawnedCartWarning = false;
}

void GameSceneJump::Update(float tick)
{
 
    if (m_CartWarning)
    {
        if (!m_HasSpawnedCartWarning &&
             m_TimeCartWarning.IsTimeUp())
        {
            if (m_Cart)
            {
                m_CartWarning->Activate(m_Cart->GetStartPos());
                m_HasSpawnedCartWarning = true;
            }
        }
        if (m_HasSpawnedCartWarning &&
            m_Cart                  &&
            m_Cart->IsActive()      &&
            m_CartWarning->IsVisible())
        {
            m_CartWarning->Deactivate();
        }
    }
    if (m_TimeCartActivetion.IsTimeUp())
    {
        if (m_Cart && !m_Cart->IsActive())
        {
            m_Cart->Start();
            m_TimeCartActivetion.timer = 0.0f;
        }
    }
   
    GameSceneExe::Update(tick);
    if (IsChange()) 
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }


}

void GameSceneJump::Finalize()
{
    GameSceneExe::Finalize();
}

