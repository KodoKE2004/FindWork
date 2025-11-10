#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Skydome.h"

void GameSceneExe::Initialize()
{
    // スピード依存でチェンジタイムの変更
    m_ChangeSceneTime *= (1.0f / m_GameSpeedMass);


}

void GameSceneExe::Update(float tick)
{
    TickCount(tick);
    if (IsTimeUp()) {
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
