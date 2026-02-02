#include "GameSceneText.h"
#include "Game.h"
#include <algorithm>
#include <random>

namespace
{
    NVector3 kButtonPos[3] = {
        NVector3(- 375.0f, - 200.0f,0.0f),
        NVector3(    0.0f, - 200.0f,0.0f),
        NVector3(  375.0f, - 200.0f,0.0f),
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
    float uvX = 0.7f;
    m_UvXOffset = (3.0f < m_UvXOffset) ? 1.0f : m_UvXOffset + uvX;
    float temp = fmodf(m_UvXOffset,1.0f);
    if (temp == 0.0f)
    {
        uvX = m_UvXOffset;
    }
    else
    {
        uvX = m_UvXOffset - temp; 
    }
    // 止まっていないスロットではUVをループ
    for (size_t i = 0; i < MESSAGE_SLOT::SLOT_SIZE; ++i)
    {
        if (!m_Clicked[i])
        {
            std::shared_ptr<Square> textObject = m_MessageSlot[i]->GetTextObject();
            textObject->SetUV(uvX, textObject->GetUV().y, textObject->GetSplit().x, textObject->GetSplit().y);
        }
    }
}

void GameSceneText::GirlReaction()
{
    float uvX = 1.0f;
    // 女の子の反応をuvXで変化させる
    const float adjectiveUvX_A    = m_MessageSlot[MESSAGE_SLOT::ADJECTIVE_A]->GetTextObject()->GetUV().x;
    const float adjectiveUvX_B    = m_MessageSlot[MESSAGE_SLOT::ADJECTIVE_B]->GetTextObject()->GetUV().x;
    
    bool high = adjectiveUvX_A == 1.0f &&
                adjectiveUvX_B == 1.0f;

    bool sad  = adjectiveUvX_A == 2.0f &&
                adjectiveUvX_B == 2.0f;

    if      (high) { uvX = 2.0f; }
    else if (sad)  { uvX = 3.0f; }
    else           { uvX = 5.0f; }

    m_Girl->SetUV(uvX, 1.0f, 5.0f, 1.0f);
    const auto uv = m_Girl->GetUV();
    const auto split = m_Girl->GetSplit();
    m_RelationData.SetTransitionTarget(m_Girl);
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
    m_RelationData.oldScene      = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear       = false;

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
    m_Boy->SetScale(600.0f,   400.0f, 1.0f);
    m_Boy->SetPos  (  0.0f, - 200.0f, 0.0f);
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
    m_isEntry     = true;
    m_isInputSlot = false;
    m_UvXOffset = 1.0f;
    m_Elapsed   = 0.0f;
    std::fill(std::begin(m_Clicked), std::end(m_Clicked), false);

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

    m_Elapsed += tick;

    // 入力処理
    if (Input::GetKeyTrigger(VK_RETURN) || Input::GetMouseTrigger(vkLEFT))
    {
        // 一定の時間が来るまでは音声のみ再生
        PlaySE("clap", 0.5f);
        
        
    } // 入力処理

    for (int i = 0; i < MESSAGE_SLOT::SLOT_SIZE; ++i)
    {
        
        MESSAGE_SLOT slot = SLOT_SIZE;
        switch (i)
        {
        case 0: slot = ADVERB     ; break;
        case 1: slot = ADJECTIVE_A; break;
        case 2: slot = ADJECTIVE_B; break;
        }
        InsideButton(i, m_MessageSlot[i], slot);
        
    }
    for (int i = 0; i < MESSAGE_SLOT::SLOT_SIZE; ++i) {
        if (m_Clicked[i]) {
            m_isInputAll = true;
        }
        else {
            m_isInputAll = false;
            break;
        }
    }
    if (m_isInputAll)
    {
        SetFastChange();
        GirlReaction();
    }
    else 
    {
        ShuffleSlotTextureUV();
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

void GameSceneText::InsideButton(int i, std::shared_ptr<Button> button, const MESSAGE_SLOT comparison)
{
    if (m_SelectedSlot != comparison)
    {
        button->SetScale(BUTTTON_BASE_SCALE);
    }
    if (button->IsInside())
    {
        if (m_SelectedSlot != comparison)
        {
            m_SelectedSlot = comparison;
            button->SetScale(BUTTTON_BASE_SCALE * 1.1f);
        }
        if (Input::GetKeyTrigger(VK_RETURN) || Input::GetMouseTrigger(vkLEFT))
        {
            float temp = fmodf(m_UvXOffset, 1.0f);
            if (temp != 0.0f)
            {
                m_UvXOffset = m_UvXOffset - temp;
            }
            auto textObject = button->GetTextObject();
            textObject->SetUV(m_UvXOffset             , textObject->GetUV().y,
                              textObject->GetSplit().x, textObject->GetSplit().y);
            
            m_Clicked[i] = true;
        }
    }
}
