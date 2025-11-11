#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Skydome.h"

void GameSceneExe::Initialize()
{
    m_RelationData = {
        false,
        m_RelationData.stageCount++,
        false,
        m_RelationData.previousScene,
        m_RelationData.nextScene
    };

    // ゲームの難易度 : スピード依存でチェンジタイムの変更
    m_ChangeSceneTime *= (1.0f / m_GameSpeedMass);
    m_ChangeFastTime = 2.0f;


}

void GameSceneExe::Update(float tick)
{
    TickCount(tick);
    if (m_isFastChange) {
        TimeCountFast(tick);

        if (IsFastTimeUp()) {
            m_isChange = true;
        }
    }
    if (IsTimeUp() && !m_isChange) {
        m_isChange = true;
    }
    if (IsChange()) {
        ChangeScenePop(TRANS_MODE::FADE, 0.2f);
    }
}

void GameSceneExe::Finalize()
{
    auto& instance = GAME_INSTANCE;
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (auto audioManager = GAME_MANAGER_AUDIO)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}

void GameSceneExe::FastChangeScene(float time)
{
    m_ChangeFastTime = time;
    m_ChangeFastTimer = 0.0f;
}
