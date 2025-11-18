#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"

#include <array>
#include <random>

namespace
{
    constexpr std::array<SCENE_NO, 3> kStaegeCandidates = {
        SCENE_NO::GAME_SLICE,
        SCENE_NO::GAME_PUSH,
        SCENE_NO::GAME_HIT
    };

    constexpr float kStageTransitionDelay = 1.0f;
}

bool GameSceneWait::s_HasFirstGameSceneWaitInitialized = false;

void GameSceneWait::PrepareNextStage()
{
    std::uniform_int_distribution<int> dist(0, static_cast<int>(kStaegeCandidates.size() - 1));
    int selectedIndex = dist(m_RandomEngine);
    const bool hasPreviousScene = 
        (m_RelationData.stageCount > 0)      &&
        (0 <= m_RelationData.lastStageIndex) &&
        (m_RelationData.lastStageIndex < static_cast<int>(kStaegeCandidates.size()));

    if (hasPreviousScene)
    {
        while (selectedIndex == m_RelationData.lastStageIndex)
        {
            selectedIndex = dist(m_RandomEngine);
        }
    }

    m_SelectedScene = kStaegeCandidates[selectedIndex];
    m_RelationData.nextScene = m_SelectedScene;
    m_RelationData.lastStageIndex = selectedIndex;
    m_ShouldTransitionToStage = true;
}

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";
    m_SelectedScene = SCENE_NO::NONE;
    
    m_IsFirstInitialized = !s_HasFirstGameSceneWaitInitialized;
    s_HasFirstGameSceneWaitInitialized = true;

    m_ShouldTransitionToStage = false;
    m_Tick = 0.0f;

    auto& instance = Game::GetInstance();

    // ライフの作成
    TextureManager* textureMgr = instance.GetInstance();

    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_LifeCount = 4;
    for(uint32_t i = 0; i < m_LifeCount; ++i)
    {
        Square* life = instance.AddObject<Square>(instance.GetCamera());
        life->SetTexture(textureMgr->GetTexture("DestroyBullet.png"));
        life->SetPos(lifePosX + ( i * 130.0f ), lifePosY, 1.0f);
        life->SetScale(100.0f, 100.0f, 1.0f);
        life->SetShader("VS_Alpha","PS_Alpha");
        life->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_MySceneObjects.emplace_back(life);
        m_LifeGame.emplace_back(life);
    }



}

void GameSceneWait::Update(float tick)
{
    // 乱数生成、ステージを厳選・二回連続で同じステージは踏まない
    m_Tick += tick;
    if (!m_ShouldTransitionToStage)
    {
        PrepareNextStage();
        m_Tick = 0.0f;
    }
    if (m_SelectedScene == SCENE_NO::NONE || 
        m_Tick < kStageTransitionDelay)
    {
        return;
    }

    switch (m_SelectedScene)
    {
    case SCENE_NO::GAME_SLICE:
        ChangeScenePush<GameSceneSlice>(TRANS_MODE::FADE, 0.3f);
        break;
    case SCENE_NO::GAME_PUSH:
        ChangeScenePush<GameScenePush>(TRANS_MODE::FADE, 0.3f);
        break;
    case SCENE_NO::GAME_HIT:
        ChangeScenePush<GameSceneHit>(TRANS_MODE::FADE, 0.3f);
        break;
    default:
        return;
    }

    m_ShouldTransitionToStage = false;
    m_SelectedScene = SCENE_NO::NONE;
}

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}
