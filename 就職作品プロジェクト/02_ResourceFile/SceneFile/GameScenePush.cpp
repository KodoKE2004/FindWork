#include "GameScenePush.h"
#include "Game.h"
#include "Collider.h"
#include "DebugUI.h"
#include <random>

float GameScenePush::GenerateActivationDelay()
{

    static std::mt19937 engine{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(1.0f, 2.0f);

    return dist(engine);
}

void GameScenePush::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameScenePush";

    // 基底クラスの初期化
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_PUSH;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    // スカイドームの設定
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    // プレイヤー
    m_Player = instance.AddObject<Player>(instance.GetCamera());
    m_Player->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Player);

    // 初期配置設定
    if (m_Player)
    {
        m_Player->SetPos(0.0f, m_PlayerGroundHeight, 0.0f);
        const float jumpOffset = m_Player->GetScale().y * PlayerJumpHeightScale;
        m_PlayerJumpApexHeight = m_PlayerGroundHeight + jumpOffset;
    }

    // カートの生成
    m_Cart = instance.AddObject<Cart>(instance.GetCamera());
    m_Cart->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Cart);
    // カートの開始パターン設定
    // パターンを登録
    m_Cart->CreateStartPattern(m_Difficulty);
    m_Cart->SetSpeedFactor(m_GameSpeedMass);
    m_Cart->SetStartPattern();
    m_TimeCartActivetion.limit = GenerateActivationDelay();

    // カート警告の生成
    m_CartWarning = instance.AddObject<CartWarning>(instance.GetCamera());
    m_CartWarning->SetTexture(textureMgr->GetTexture("Plane.png"));
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

void GameScenePush::Update(float tick)
{
    m_Player->m_HitResult.SetHitResult(
        Math::Collider2D::isHitSquareSquare(*m_Player, *m_Cart)
    );

    if (m_Player->m_HitResult.isTriggered())
    {
        MyDebugLog(Debug::Log("当たった");)
        m_RelationData.isClear = false;
    }

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
    if (IsChange()) {
        ChangeScenePop(TRANS_MODE::FADE, 0.2f);
    }


}

void GameScenePush::Finalize()
{
    GameSceneExe::Finalize();
}

