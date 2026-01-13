#include "GameSceneText.h"
#include "Game.h"

void GameSceneText::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneText";
#endif
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_TEXT;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance = Game::GetInstance();

    //===============================
    //      シーン内オブジェクト生成
    //===============================
    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_True   = instance.AddObject<Button>();
    m_FalseA = instance.AddObject<Button>();
    m_FalseB = instance.AddObject<Button>();
    
    m_MySceneObjects.emplace_back(m_True);
    m_MySceneObjects.emplace_back(m_FalseA);
    m_MySceneObjects.emplace_back(m_FalseB);
}

void GameSceneText::Update(float tick)
{
    GameSceneExe::Update(tick);
    if (IsChange())
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        // ChangeScenePop(GameToWait);
    }

}

void GameSceneText::Finalize()
{
    GameSceneExe::Finalize();
}
