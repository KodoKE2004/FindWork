#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Skydome.h"

void GameSceneExe::Initialize()
{
    // 初期化
    m_Difficulty    = 0;
    m_GameSpeedMass = 1.0f;
    m_TimeChangeScene.timer = 5.0f;
    m_isChange     = false;
    m_isFastChange = false;
    m_ChangeFastTimer = 0.0f;
    
    //===============================
    //   ゲームスピード倍率設
    //   n && 8 == 0 なら難易度アップ 
    //   n && 4 == 0 ならスピードアップ
    //===============================
    
    ++m_RelationData.stageCount;
    // 難易度 0 ~
    if (m_RelationData.stageCount % 8 == 0) {
        int difficulty = m_RelationData.stageCount / 8;
        m_Difficulty = difficulty;
    }
    // スピード
    else if(m_RelationData.stageCount % 4 == 0){
        float speedMass = ((float)m_RelationData.stageCount / 8.0f) + 1.0f;
        m_GameSpeedMass = 1.0f + (speedMass * 0.2f);
    }

    // 値の反映
    m_TimeChangeScene.limit /= m_GameSpeedMass;
    m_ChangeFastTime   = 2.0f * (1 - m_GameSpeedMass);
    m_TimerList.clear();
    SetTimer(&m_TimeChangeScene.timer);

}

void GameSceneExe::Update(float tick)
{
    bool clearConect =  m_RelationData.isClear &&
                       (m_isFastChange || m_TimeChangeScene.IsTimeUp());

    if (m_isFastChange) {
        TimeCountFast(tick);

        if (IsFastTimeUp()) {
            m_isChange = true;
        }
    }
    if (m_TimeChangeScene.IsTimeUp() &&
        !m_isChange) {
        m_isChange = true;
    }
    

    CountTimer(tick);
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
