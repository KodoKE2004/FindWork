#include "GameSceneText.h"
#include "Game.h"
#include <algorithm>
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

    std::array<float, 2> ShuffleButtonIndices(std::array<float, 2> indices)
    {
        static std::mt19937 engine{ std::random_device{}() };
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

    m_Number = ShuffleButtonIndices();
    for (size_t i = 0; i < 3; ++i)
    {
        float uvY = static_cast<float>(i + 1);
        m_GameRhythm [i] = kGameRhythm[m_Number[0]][i] * GetOneBeat();

        m_MessageSlot[i] = instance.AddObject<Button>();
        m_MessageSlot[i]->SetName("MessageSlot " + std::to_string(i));
        m_MessageSlot[i]->SetTexture(textureMgr->GetTexture("Button/Frame.png"));
        m_MessageSlot[i]->SetBaseScale(NVector3(240.0f, 80.0f, 1.0f));
        m_MessageSlot[i]->SetTextTexture(textureMgr->GetTexture("Button/Text/MessageSlot.png"));
        m_MessageSlot[i]->GetTextObject()->SetUV(2.0f, uvY, 3.0f, 3.0f);
        m_MessageSlot[i]->SetPos(kButtonPos[i]);
        
        m_MySceneObjects.emplace_back(m_MessageSlot[i]);
        m_MySceneObjects.emplace_back(m_MessageSlot[i]->GetTextObject());
    }

    m_InputIndex = 0;
    m_CurrentRhythmIndex = 0;
    m_isEntry = true;
    m_isInput = false;
    m_UvXOffset = 1.0f;
    m_Elapsed = 0.0f;
    std::fill(std::begin(m_Clicked), std::end(m_Clicked), false);

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

    PlayParams clapParam{};
    m_AudioList.emplace("clap", AudioConfig(L"SE/HandClap.wav" , clapParam, false, false));
    
    PlayParams whistleParam{};
    m_AudioList.emplace("whistle", AudioConfig(L"SE/Whistle.wav"  , whistleParam, false, false));

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

    m_Elapsed += tick;
    if (Input::GetKeyTrigger(VK_RETURN) || Input::GetMouseTrriger(vkLEFT))
    {
        // 一定の時間が来るまでは音声のみ再生
        PlaySE("clap", 0.5f);
        size_t index = m_InputIndex;
        if (index < 3)
        {
            const float tolerance = 0.4f;
            bool justTiming = (m_GameRhythm[index] - tolerance) <= m_Elapsed && 
                               m_Elapsed <= (m_GameRhythm[index] + tolerance);

            auto textObject = m_MessageSlot[index]->GetTextObject();

            // 正しいタイミングで押された場合
            if (m_isInput && justTiming) 
            {
                textObject->SetUV( 1.0f,                     textObject->GetUV().y, 
                                   textObject->GetSplit().x, textObject->GetSplit().y);
            }
            // 間違ったタイミングで押された場合
            else if (m_isInput && !justTiming) {
                std::array<float, 2> falseUV_X = {2.0f,3.0f};
                falseUV_X = ShuffleButtonIndices(falseUV_X);
                textObject->SetUV( falseUV_X[0]            , textObject->GetUV().y,
                                   textObject->GetSplit().x, textObject->GetSplit().y);
            }
            if (m_isInput)
            {
                m_Clicked[index] = true;
                ++m_InputIndex ;
            }
        }
    }

    m_UvXOffset = 4.0f <= m_UvXOffset ? 
                  1.0f :  m_UvXOffset + 1.0f;
    for (size_t i = 0; i < 3; ++i)
    {
        if (!m_Clicked[i])
        {
            auto textObject = m_MessageSlot[i]->GetTextObject();
            textObject->SetUV(m_UvXOffset, textObject->GetUV().y, textObject->GetSplit().x, textObject->GetSplit().y);
        }
    }

    // リズムに合わせて効果音を鳴らす
    if (m_Elapsed >= m_GameRhythm[m_CurrentRhythmIndex] && m_isEntry)
    {
        PlaySE("whistle", 0.5f);
        
        // 次のリズムをセット
        float nextBeat = 4.0f + kGameRhythm[m_Number[0]][m_CurrentRhythmIndex];

        // 入力受付フラグを立てる
        // 指定された3拍目以降から受付開始
        if(m_CurrentRhythmIndex == 2) {
            m_isInput = true;
        }

        m_GameRhythm[m_CurrentRhythmIndex] = nextBeat * GetOneBeat();
        m_CurrentRhythmIndex++;

        // 最後のリズムがが経過したらエントリーフラグを立てる
        if (m_Elapsed >= m_GameRhythm[2]) {
            m_isEntry = false;
        }

        if(m_CurrentRhythmIndex >= 3) {
            m_CurrentRhythmIndex = 0;
        }

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
