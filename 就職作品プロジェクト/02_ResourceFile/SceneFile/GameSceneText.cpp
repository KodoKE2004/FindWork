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
    m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlNormal.png"));
    m_Girl->SetScale(400.0f,600.0f,1.0f);
    m_Girl->SetPos  (  0.0f, 50.0f,0.0f);
    m_MySceneObjects.emplace_back(m_Girl);

    m_Boy = instance.AddObject<Square>();
    m_Boy->SetName("m_Boy");
    m_Boy->SetTexture(textureMgr->GetTexture("LoveLatterOffer.png"));
    m_Boy->SetScale(600.0f,400.0f,1.0f);
    m_Boy->SetPos  (0.0f, -200.0f, 0.0f);
    m_MySceneObjects.emplace_back(m_Boy);



    m_True   = instance.AddObject<Button>();
    m_FalseA = instance.AddObject<Button>();
    m_FalseB = instance.AddObject<Button>();
    
    m_True  ->SetName("m_True");
    m_FalseA->SetName("m_FalseA");
    m_FalseB->SetName("m_FalseB");

    m_True  ->SetTexture(textureMgr->GetTexture("Button/Frame.png"));
    m_FalseA->SetTexture(textureMgr->GetTexture("Button/Frame.png"));
    m_FalseB->SetTexture(textureMgr->GetTexture("Button/Frame.png"));

    m_True  ->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_FalseA->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
    m_FalseB->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));

    m_True  ->SetTextTexture(textureMgr->GetTexture("Button/Text/LoveYouTrue.png"));
    m_FalseA->SetTextTexture(textureMgr->GetTexture("Button/Text/LoveYouFalseA.png"));
    m_FalseB->SetTextTexture(textureMgr->GetTexture("Button/Text/LoveYouFalseB.png"));

    std::array<size_t, 3> number = ShuffleButtonIndices();

    m_TrueTargetPos   = gButtonPos[number[0]];
    m_FalseATargetPos = gButtonPos[number[1]];
    m_FalseBTargetPos = gButtonPos[number[2]];

    const NVector3 fadeOffset(0.0f, kButtonFadeOffsetY, 0.0f);
    m_True->SetPos(m_TrueTargetPos + fadeOffset);
    m_FalseA->SetPos(m_FalseATargetPos + fadeOffset);
    m_FalseB->SetPos(m_FalseBTargetPos + fadeOffset);

    m_True  ->SetColor(1.0f, 1.0f, 1.0f, 0.0f);
    m_FalseA->SetColor(1.0f, 1.0f, 1.0f, 0.0f);
    m_FalseB->SetColor(1.0f, 1.0f, 1.0f, 0.0f);

    m_True  ->SetTextColor(1.0f, 1.0f, 1.0f, 0.0f);
    m_FalseA->SetTextColor(1.0f, 1.0f, 1.0f, 0.0f);
    m_FalseB->SetTextColor(1.0f, 1.0f, 1.0f, 0.0f);

    m_MySceneObjects.emplace_back(m_True);
    m_MySceneObjects.emplace_back(m_FalseA);
    m_MySceneObjects.emplace_back(m_FalseB);

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

void GameSceneText::Update(float tick)
{
    GameSceneExe::Update(tick);
    AudioManager* audioMgr = Game::GetInstance();

    TimerUIUpdate(tick);

    m_ButtonFadeTimer = std::min(m_ButtonFadeTimer + tick, m_ButtonFadeDuration);
    const float rawFadeProgress = std::clamp(m_ButtonFadeTimer / m_ButtonFadeDuration, 0.0f, 1.0f);
    const float fadeProgress = rawFadeProgress * rawFadeProgress * (3.0f - 2.0f * rawFadeProgress);

    auto applyButtonFade = [&](const std::shared_ptr<Button>& button, const NVector3& targetPos)
        {
            if (!button)
            {
                return;
            }
            const NVector3 startPos = targetPos + NVector3(0.0f, kButtonFadeOffsetY, 0.0f);
            const NVector3 nextPos = startPos + (targetPos - startPos) * fadeProgress;
            button->SetPos(nextPos);
            button->SetColor(1.0f, 1.0f, 1.0f, fadeProgress);
            button->SetTextColor(1.0f, 1.0f, 1.0f, fadeProgress);
        };

    applyButtonFade(m_True  , m_TrueTargetPos);
    applyButtonFade(m_FalseA, m_FalseATargetPos);
    applyButtonFade(m_FalseB, m_FalseBTargetPos);

    if (!trigger)
    {
        // 最後に触れたButtonを検出する
        InsideButton(m_SelectedButton, m_True  , BUTTON_TRUE   );
        InsideButton(m_SelectedButton, m_FalseA, BUTTON_FALSE_A);
        InsideButton(m_SelectedButton, m_FalseB, BUTTON_FALSE_B);
    }
    
    if (m_BeatTimer.GetRestBeats() == 3 && !trigger)
    {
        trigger = true;
        TextureManager* textureMgr = Game::GetInstance();
        m_Boy->SetTexture(textureMgr->GetTexture("LoveLatterHeldOut.png"));
        if (m_SelectedButton == BUTTON_TRUE)
        {
            StageClear();
            PlaySE("true", 0.5f);
            m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlGlad.png"));
        }
        else
        {
            StageFail();
            PlaySE("false", 0.5f);
            m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlWhy.png"));
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
