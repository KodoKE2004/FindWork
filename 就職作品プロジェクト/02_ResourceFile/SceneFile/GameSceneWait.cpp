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
}

void GameSceneWait::PrepareNextStage()
{
    static std::mt19937 engine{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(0, static_cast<int>(kStaegeCandidates.size() - 1));
    int selectedIndex = dist(engine);
    const bool hasPreviousScene = 
        (m_RelationData.stageCount > 0)      &&
        (0 <= m_RelationData.lastStageIndex) &&
        (m_RelationData.lastStageIndex < static_cast<int>(kStaegeCandidates.size()));

    if (hasPreviousScene)
    {
        while (selectedIndex == m_RelationData.lastStageIndex)
        {
            selectedIndex = dist(engine);
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
    m_ShouldTransitionToStage = false;
}

void GameSceneWait::Update(float tick)
{
    // 乱数生成、ステージを厳選・二回連続で同じステージは踏まない
    (void)tick;
    if (!m_ShouldTransitionToStage)
    {
        PrepareNextStage();
    }
    if (m_SelectedScene == SCENE_NO::NONE)
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
