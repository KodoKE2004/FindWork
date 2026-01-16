#include "GameSceneText.h"
#include "Game.h"
#include <algorithm>
#include <array>
#include <random>

namespace
{
    NVector3 kButtonPos[3] = {
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

    // 4パターンのリズム配置 
    float kGameRhythm[3][3] = {
        { 1.0f, 2.0f, 3.0f },
        { 1.0f, 2.5f, 3.0f },
        { 1.0f, 1.5f, 2.0f },
    };
}

void GameSceneText::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneText";
#endif
    GameSceneExe::SetBaseBeatCount(BASE_BEATS + 4);
    GameSceneExe::Initialize();
    m_ButtonFadeDuration = static_cast<float>(m_RelationData.rhythmBeat.GetBeatConst().m_TicksPerBeat);

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_TEXT;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance; 
    //===============================
    //      シーン内オブジェクト生成
    //===============================

    std::shared_ptr<Square> backGround = instance.AddObject<Square>();
    backGround->SetName("backGround");
    backGround->SetTexture(textureMgr->GetTexture("BackGround/School.png"));
    backGround->SetScale(1280.0f,720.0f,1.0f);
    m_MySceneObjects.emplace_back(backGround);

    m_Girl = instance.AddObject<Square>();
    m_Girl->SetName("m_Girl");
    m_Girl->SetTexture(textureMgr->GetTexture("Girl.png"));
    m_Girl->SetUV(1.0f,1.0f,5.0f,1.0f);
    m_Girl->SetScale(400.0f,600.0f,1.0f);
    m_Girl->SetPos  (  0.0f, 50.0f,0.0f);
    m_MySceneObjects.emplace_back(m_Girl);

    m_Boy = instance.AddObject<Square>();
    m_Boy->SetName("m_Boy");
    m_Boy->SetTexture(textureMgr->GetTexture("LoveLatterOffer.png"));
    m_Boy->SetScale(600.0f,400.0f,1.0f);
    m_Boy->SetPos  (0.0f, -200.0f, 0.0f);
    m_MySceneObjects.emplace_back(m_Boy);

    m_Adverb     = instance.AddObject<Button>();
    m_AdjectiveA = instance.AddObject<Button>();
    m_AdjectiveB = instance.AddObject<Button>();
    
    m_Adverb    ->SetName("m_True");
    m_AdjectiveA->SetName("m_FalseA");
    m_AdjectiveB->SetName("m_FalseB");

    m_Adverb    ->SetTexture(textureMgr->GetTexture("Button/Frame.png"));
    m_AdjectiveA->SetTexture(textureMgr->GetTexture("Button/Frame.png"));
    m_AdjectiveB->SetTexture(textureMgr->GetTexture("Button/Frame.png"));

    m_Adverb    ->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_AdjectiveA->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_AdjectiveB->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));

    m_Adverb    ->SetTextTexture(textureMgr->GetTexture("Button/Text/MessageSlot.png"));
    m_AdjectiveA->SetTextTexture(textureMgr->GetTexture("Button/Text/MessageSlot.png"));
    m_AdjectiveB->SetTextTexture(textureMgr->GetTexture("Button/Text/MessageSlot.png"));

    m_Adverb    ->GetTextObject()->SetUV(1.0f, 1.0f, 3.0f, 3.0f);
    m_AdjectiveA->GetTextObject()->SetUV(1.0f, 2.0f, 3.0f, 3.0f);
    m_AdjectiveB->GetTextObject()->SetUV(1.0f, 3.0f, 3.0f, 3.0f);

    m_Adverb    ->SetPos(kButtonPos[0]);
    m_AdjectiveA->SetPos(kButtonPos[1]);
    m_AdjectiveB->SetPos(kButtonPos[2]);

    m_MySceneObjects.emplace_back(m_Adverb);
    m_MySceneObjects.emplace_back(m_AdjectiveA);
    m_MySceneObjects.emplace_back(m_AdjectiveB);

    m_MySceneObjects.emplace_back(m_Adverb    ->GetTextObject());
    m_MySceneObjects.emplace_back(m_AdjectiveA->GetTextObject());
    m_MySceneObjects.emplace_back(m_AdjectiveB->GetTextObject());

    std::array<size_t, 3> number = ShuffleButtonIndices();

    m_GameRhythm[0] = kGameRhythm[number[0]][0] * GetOneBeat();
    m_GameRhythm[1] = kGameRhythm[number[0]][1] * GetOneBeat();
    m_GameRhythm[2] = kGameRhythm[number[0]][2] * GetOneBeat();

    m_TimerUI = instance.AddObject<Timer>();
    m_TimerUI->SetName("m_TimerUI");
    m_TimerUI->SetPos(500.0f, 300.0f, 0.0f);
    m_TimerUI->SetTexture(textureMgr->GetTexture("Clock.png"));
    m_MySceneObjects.emplace_back(m_TimerUI);

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

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
            }
        }
    }
}

float i = 0; 
void GameSceneText::Update(float tick)
{
    GameSceneExe::Update(tick);
    AudioManager* audioMgr = Game::GetInstance();

    
    if (Input::GetKeyTrigger(VK_LEFT))
    {
        i += 1.0f;
    }
    if (Input::GetKeyTrigger(VK_RIGHT))
    {
        i -= 1.0f;
    }



    if (IsChange())
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
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
