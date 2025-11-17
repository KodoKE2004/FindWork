#include "ResultScene.h"
#include "Game.h"
#include "DebugUI.h"

void ResultScene::Initialize()
{
    auto& instance = Game::GetInstance();
    DebugUI::TEXT_CurrentScene = "ResultScene";

    m_RelationData = {
        m_RelationData.isClear,
        m_RelationData.stageCount,
        m_RelationData.requestRetry,
        m_RelationData.previousScene,
        m_RelationData.nextScene
    };

}

void ResultScene::Update(float tick)
{
}

void ResultScene::Finalize()
{
    auto& instance = Game::GetInstance();
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
