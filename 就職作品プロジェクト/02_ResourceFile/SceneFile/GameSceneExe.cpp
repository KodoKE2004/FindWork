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
        m_RelationData.stageCount,
        false,
        m_RelationData.previousScene,
        m_RelationData.nextScene
    };
    m_RelationData.stageCount++;
    
    // ゲームスピード倍率設
    // 難易度アップ　定数用意からステージごとに難易度アップ
    // !(n % 8) == 0 && n && 4 ならスピードアップ
    if (m_RelationData.stageCount % 8 == 0) {
        int difficulty = m_RelationData.stageCount / 8;
        m_DifficultLevel = difficulty + 1;
    }
    else if(m_RelationData.stageCount % 4 == 0){
        float speedMass = ((float)m_RelationData.stageCount / 8.0f) + 1.0f;
        m_GameSpeedMass = 1.0f + (speedMass * 0.2f);
    }
    m_ChangeSceneTime *= (1.0f * m_GameSpeedMass);
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

void GameSceneExe::FastChangeScene(float time)
{
    m_ChangeFastTime = time;
    m_ChangeFastTimer = 0.0f;
}
