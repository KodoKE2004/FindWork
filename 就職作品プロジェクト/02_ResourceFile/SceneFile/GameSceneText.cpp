#include "GameSceneText.h"
#include "Game.h"

namespace
{
    NVector3 gButtonPos[3] = {
        NVector3(- 300.0f,0.0f,0.0f),
        NVector3(    0.0f,0.0f,0.0f),
        NVector3(  300.0f,0.0f,0.0f),
    };
}

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
    TextureManager* textureMgr = instance; 
    //===============================
    //      シーン内オブジェクト生成
    //===============================
    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_True   = instance.AddObject<Button>();
    m_FalseA = instance.AddObject<Button>();
    m_FalseB = instance.AddObject<Button>();
    
    m_True  ->SetName("m_True");
    m_FalseA->SetName("m_FalseA");
    m_FalseB->SetName("m_FalseB");

    m_True  ->SetTexture(textureMgr->GetTexture("Text/Frame.png"));
    m_FalseA->SetTexture(textureMgr->GetTexture("Text/Frame.png"));
    m_FalseB->SetTexture(textureMgr->GetTexture("Text/Frame.png"));

    m_True  ->SetBaseScale(NVector3(450.0f * 0.8f, 300.0f * 0.8f, 1.0f));
    m_FalseA->SetBaseScale(NVector3(450.0f * 0.8f, 300.0f * 0.8f, 1.0f));
    m_FalseB->SetBaseScale(NVector3(450.0f * 0.8f, 300.0f * 0.8f, 1.0f));

    m_True  ->SetTextTexture(textureMgr->GetTexture("Text/LoveYouTrue.png"));
    m_FalseA->SetTextTexture(textureMgr->GetTexture("Text/LoveYouFalseA.png"));
    m_FalseB->SetTextTexture(textureMgr->GetTexture("Text/LoveYouFalseB.png"));

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
