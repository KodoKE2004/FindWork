#include "ResultScene.h"
#include "SceneList.h"
#include "Game.h"
#include "DebugUI.h"

void ResultScene::Initialize()
{
    auto& instance = Game::GetInstance();
    DebugUI::TEXT_CurrentScene = "ResultScene";

    TextureManager* textureMgr = instance;
    // Skydome初期化 
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_ButtonToTitle = instance.AddObject<Square>();
    m_ButtonToTitle->SetName("m_ButtonToTitle");
    m_ButtonToTitle->SetTexture(textureMgr->GetTexture("ButtonToTitle.png"));
    m_ButtonToTitle->SetPos(0.0f, -100.0f, 0.0f);
    m_ButtonToTitle->SetScale(940.0f, 100.0f, 1.0f);
    m_ButtonToTitle->SetShader("VS_Alpha", "PS_Alpha");
    m_ButtonToTitle->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_MySceneObjects.emplace_back(m_ButtonToTitle);

    m_ButtonRetry = instance.AddObject<Square>();
    m_ButtonRetry->SetName("m_ButtonToRetry");
    m_ButtonRetry->SetTexture(textureMgr->GetTexture("ButtonRetry.png"));
    m_ButtonRetry->SetPos(0.0f, -200.0f, 0.0f);
    m_ButtonRetry->SetScale(940.0f, 100.0f, 1.0f);
    m_ButtonRetry->SetShader("VS_Alpha", "PS_Alpha");
    m_ButtonRetry->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_MySceneObjects.emplace_back(m_ButtonRetry);

    PlayParams corsorParam{};
    corsorParam.volume = DEFAULT_VALUME;
    m_AudioList.emplace("moveCorsor", AudioConfig(L"SE/MoveCorsor.wav", corsorParam, false, false));



    MyDebugLog(std::cout << "クリアステージ数" << m_RelationData.stageCount << std::endl;)
}

void ResultScene::Update(float tick)
{
    m_DurationCuror += tick;

    if (Input::GetKeyTrigger(VK_UP) || Input::GetKeyTrigger(VK_DOWN) ||
        Input::GetKeyTrigger(VK_W)  || Input::GetKeyTrigger(VK_S)){
        m_isCorsorButtonToTitle ^= true;
        // SEの再生
        if (AudioManager* audioMgr = Game::GetInstance())
        {
            if (auto it = m_AudioList.find("moveCorsor"); it != m_AudioList.end())
            {
                auto params = it->second.params;
                if (it->second.loop)
                {
                    params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
                }
                audioMgr->Play("moveCorsor", params);
            }
            else
            {
                audioMgr->Play("moveCorsor");
            }
        }
    }
    
    // PressEnterをチカチカさせる
    // 一定時間経過でアルファ値をいじる
    if (m_DurationCuror >= AlphaChangeTimer)
    {
        float alpha = 1.0f;
        if (m_isCorsorButtonToTitle)
        {
            alpha = m_ButtonToTitle->GetColor().w;
            m_ButtonToTitle->SetColor(1.0f,1.0f,1.0f,1.0f - alpha)  ;
            m_ButtonRetry->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            alpha = m_ButtonRetry->GetColor().w;
            m_ButtonRetry->SetColor(1.0f, 1.0f, 1.0f, 1.0f - alpha);
            m_ButtonToTitle->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
        m_DurationCuror = 0.0f;
    }

    if (Input::GetKeyTrigger(VK_RETURN))
    {
        // SEの再生
        if (AudioManager* audioMgr = Game::GetInstance())
        {
            if (auto it = m_AudioList.find("enter"); it != m_AudioList.end())
            {
                auto params = it->second.params;
                if (it->second.loop)
                {
                    params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
                }
                audioMgr->Play("enter", params);
            }
            else
            {
                audioMgr->Play("enter");
            }

            audioMgr->StopAllByName("bgm", false);
        }
        if (m_isCorsorButtonToTitle)
        {
            // タイトルへ戻る
            m_RelationData.previousScene = SCENE_NO::RESULT;
            m_RelationData.nextScene     = SCENE_NO::TITLE;
            SceneTransitionParam transition{ TRANS_MODE::FADE, 0.3f, EASING_TYPE::NONE };
            ChangeScenePush<TitleScene>(transition, transition);
        }
        else
        {
            // ゲームを最初から始める
            // シーンに繋ぐ情報は基底初期化後の一番最初に設定
            m_RelationData.previousScene = SCENE_NO::RESULT;
            m_RelationData.nextScene     = SCENE_NO::GAME_WAIT;
            m_RelationData.isClear = true;
            m_RelationData.stageCount = 0;
            m_RelationData.gameLife   = 4;
            SceneTransitionParam transition{ TRANS_MODE::FADE, 0.3f, EASING_TYPE::NONE };
            ChangeScenePop(transition, transition);
        }


    }
    
    
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
