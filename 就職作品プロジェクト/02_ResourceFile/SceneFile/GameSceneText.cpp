#include "GameSceneText.h"
#include "Game.h"
#include <algorithm>
#include <array>
#include <random>

namespace
{
    NVector3 gButtonPos[3] = {
        NVector3(- 375.0f,0.0f,0.0f),
        NVector3(    0.0f,0.0f,0.0f),
        NVector3(  375.0f,0.0f,0.0f),
    };

    std::array<size_t, 3> ShuffleButtonIndices()
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::array<size_t, 3> indices{ 0, 1, 2 };
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }
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

    std::shared_ptr<Square> backGround = instance.AddObject<Square>();
    backGround->SetName("backGround");
    backGround->SetTexture(textureMgr->GetTexture("BackGround/School.png"));
    backGround->SetScale(1280.0f,720.0f,1.0f);
    m_MySceneObjects.emplace_back(backGround);

    m_Boy = instance.AddObject<Square>();
    m_Boy->SetName("m_Boy");
    m_Boy->SetTexture(textureMgr->GetTexture("LoveLatterOffer.png"));
    m_Boy->SetScale(600.0f,400.0f,1.0f);
    m_Boy->SetPos  (0.0f, -200.0f, 0.0f);
    m_MySceneObjects.emplace_back(m_Boy);

    m_Girl = instance.AddObject<Square>();
    m_Girl->SetName("m_Girl");
    m_Girl->SetTexture(textureMgr->GetTexture("LoveLatterOffer.png"));
    m_Girl->SetScale(600.0f,400.0f,1.0f);
    m_MySceneObjects.emplace_back(m_Girl);


    m_True   = instance.AddObject<Button>();
    m_FalseA = instance.AddObject<Button>();
    m_FalseB = instance.AddObject<Button>();
    
    m_True  ->SetName("m_True");
    m_FalseA->SetName("m_FalseA");
    m_FalseB->SetName("m_FalseB");

    m_True  ->SetTexture(textureMgr->GetTexture("Text/Frame.png"));
    m_FalseA->SetTexture(textureMgr->GetTexture("Text/Frame.png"));
    m_FalseB->SetTexture(textureMgr->GetTexture("Text/Frame.png"));

    m_True  ->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_FalseA->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_FalseB->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));

    m_True  ->SetTextTexture(textureMgr->GetTexture("Text/LoveYouTrue.png"));
    m_FalseA->SetTextTexture(textureMgr->GetTexture("Text/LoveYouFalseA.png"));
    m_FalseB->SetTextTexture(textureMgr->GetTexture("Text/LoveYouFalseB.png"));

    std::array<size_t, 3> number = ShuffleButtonIndices();

    m_True  ->SetPos(gButtonPos[number[0]]);
    m_FalseA->SetPos(gButtonPos[number[1]]);
    m_FalseB->SetPos(gButtonPos[number[2]]);

    m_MySceneObjects.emplace_back(m_True);
    m_MySceneObjects.emplace_back(m_FalseA);
    m_MySceneObjects.emplace_back(m_FalseB);

    PlayParams insideParam{};
    m_AudioList.emplace("rhythm", AudioConfig(L"SE/Rhythm.wav", insideParam, false, false));

    PlayParams trueParam{};
    m_AudioList.emplace("true", AudioConfig(L"SE/ExeTrue.wav", trueParam, false, false));

    PlayParams falseParam{};
    m_AudioList.emplace("false", AudioConfig(L"SE/ExeFalse.wav", falseParam, false, false));

    if (AudioManager* audioMgr = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            if (!audioMgr->Add(key, config.filePath)) {
                continue;
            }
            if (config.autoPlay)
            {
                auto params = config.params;
                if (config.loop)
                {
                    params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
                }
                audioMgr->Play(key, params);
            }
        }
    }
}

void GameSceneText::Update(float tick)
{
    GameSceneExe::Update(tick);
    AudioManager* audioMgr = Game::GetInstance();

    // 最後に触れたButtonを検出する
    InsideButton(m_SelectedButton, m_True  , BUTTON_TRUE   );
    InsideButton(m_SelectedButton, m_FalseA, BUTTON_FALSE_A);
    InsideButton(m_SelectedButton, m_FalseB, BUTTON_FALSE_B);
    
    if (m_BeatTimer.GetRestBeats() == 3 && !trigger)
    {
        trigger = true;
        TextureManager* textureMgr = Game::GetInstance();
        m_Boy->SetTexture(textureMgr->GetTexture("LoveLatterHeldOut.png"));
        if (m_SelectedButton == BUTTON_TRUE)
        {
            StageClear();
            PlaySE("true", 0.5f);
        }
        else
        {
            StageFail();
            PlaySE("false", 0.5f);
        }
    }


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

void GameSceneText::InsideButton(LAST_DRAG& lastDrag, std::weak_ptr<Button> button, const LAST_DRAG comparison)
{   
    auto temp = button.lock();
    if (lastDrag != comparison)
    {
        temp->SetScale(BUTTTON_BASE_SCALE);
    }
    if (temp->IsInside())
    {
        if (lastDrag != comparison)
        {
            lastDrag = comparison;
            PlaySE("rhythm", 0.5f);
            temp->SetScale(BUTTTON_BASE_SCALE * 1.1f);
        }
    }
}
