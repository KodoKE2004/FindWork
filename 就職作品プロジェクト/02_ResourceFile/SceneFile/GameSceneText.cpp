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

void GameSceneText::ShuffleSlotTextureUV()
{
    m_UvXOffset = 4.0f <= m_UvXOffset ?
                  1.0f :  m_UvXOffset + 1.0f;

    // 止まっていないスロットではUVをループ
    for (size_t i = 0; i < MESSAGE_SLOT::SLOT_SIZE; ++i)
    {
        if (!m_Clicked[i])
        {
            auto textObject = m_MessageSlot[i]->GetTextObject();
            textObject->SetUV(m_UvXOffset, textObject->GetUV().y, textObject->GetSplit().x, textObject->GetSplit().y);
        }
    }
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
    for (int i = 0; i < MESSAGE_SLOT::SLOT_SIZE; ++i)
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
    m_isInputSlot = false;
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
    AudioManager*   audioMgr   = Game::GetInstance();
    TextureManager* textureMgr = Game::GetInstance();
    m_Elapsed += tick;

    // 入力処理
    if (Input::GetKeyTrigger(VK_RETURN) || Input::GetMouseTrriger(vkLEFT))
    {
        // 一定の時間が来るまでは音声のみ再生
        PlaySE("clap", 0.5f);
        size_t index = m_InputIndex;
        // SLOTの数分入力の受付
        if (index < MESSAGE_SLOT::SLOT_SIZE)
        {
            // 入力許容の時間
            const float tolerance = 0.4f;
            bool justTiming = (m_GameRhythm[index] - tolerance) <= m_Elapsed && 
                               m_Elapsed <= (m_GameRhythm[index] + tolerance);

            auto textObject = m_MessageSlot[index]->GetTextObject();

            // 正しいタイミングで押された場合
            if (m_isInputSlot && justTiming) 
            {
                textObject->SetUV( 1.0f,                     textObject->GetUV().y, 
                                   textObject->GetSplit().x, textObject->GetSplit().y);
            }
            // 間違ったタイミングで押された場合
            else if (m_isInputSlot && !justTiming) {
                std::array<float, 2> falseUV_X = {2.0f, 3.0f};
                falseUV_X = ShuffleButtonIndices(falseUV_X);
                textObject->SetUV( falseUV_X[0]            , textObject->GetUV().y,
                                   textObject->GetSplit().x, textObject->GetSplit().y);
            }
            if (m_isInputSlot)
            {
                m_Clicked[index] = true;
                ++m_InputIndex ;
            }
            if (m_InputIndex == MESSAGE_SLOT::SLOT_SIZE) {
                m_isInputAll = true;
            }
        }
    } // 入力処理


    if (m_isInputAll)
    {
        // 女の子の反応のtextureを用意
        // 言葉次第で反応が変化
        int msgSlotAdjective = static_cast<int> (m_MessageSlot[MESSAGE_SLOT::ADJECTIVE_A]->GetUV().x 
                                               + m_MessageSlot[MESSAGE_SLOT::ADJECTIVE_B]->GetUV().x);

        float msgSlotAdverb    = m_MessageSlot[MESSAGE_SLOT::ADVERB]->GetUV().x;
        
        switch (msgSlotAdjective)
        {
        case 2  : 
            if (msgSlotAdverb < 3.0f){
                m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlHigh.png"));   
            }
            else {
                m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlGlad.png"));   
            }
        break;
        case 4  : m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlSad.png") );   break;
        default : m_Girl->SetTexture(textureMgr->GetTexture("Girl/LoveLatterGirlWhy.png"));    break;
        }

    }
    else 
    {
        ShuffleSlotTextureUV();
    }

    // リズムに合わせて効果音を鳴らす
    if (m_Elapsed >= m_GameRhythm[m_CurrentRhythmIndex] && m_isEntry)
    {
        PlaySE("whistle", 0.5f);
        
        // 次の小節に使いまわしする 4拍1小節
        float nextBeat = 4.0f + kGameRhythm[m_Number[0]][m_CurrentRhythmIndex];

        // 入力受付フラグを立てる
        // 指定された3拍目以降から受付開始
        if(m_CurrentRhythmIndex == MESSAGE_SLOT::ADJECTIVE_B) {
            m_isInputSlot = true;
        }
        
        // 次の小節のTickを算出
        m_GameRhythm[m_CurrentRhythmIndex] = nextBeat * GetOneBeat();
        m_CurrentRhythmIndex++;

        // 最後のリズムがが経過したらエントリーフラグを立てる
        if (m_Elapsed >= m_GameRhythm[MESSAGE_SLOT::ADJECTIVE_B]) {
            m_isEntry = false;
        }

        if(m_CurrentRhythmIndex >= MESSAGE_SLOT::SLOT_SIZE) {
            m_CurrentRhythmIndex = 0;
        }

    } // リズムに合わせて効果音を鳴らす


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
