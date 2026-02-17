#include "ResultScene.h"
#include "SceneList.h"
#include "Game.h"
#include "DebugUI.h"

using namespace Calculator::Easing;

ResultScene::ResultScene(Camera& cam) : Scene(cam)
{
}

void ResultScene::Initialize()
{
    auto& instance = Game::GetInstance();
    DebugUI::TEXT_CurrentScene = "ResultScene";

    TextureManager* textureMgr = instance;
    // Skydome初期化 
    m_Skydome = AddObject<Skydome>(instance.GetCamera());
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);

    m_ButtonToTitle = AddObject<MouseObject>(instance.GetCamera());
    m_ButtonToTitle->SetName("m_ButtonToTitle");
    m_ButtonToTitle->SetTexture(textureMgr->GetTexture("Button/Text/ToTitle.png"));
    m_ButtonToTitle->SetPos(0.0f, -100.0f, 0.0f);
    m_ButtonToTitle->SetScale(940.0f, 100.0f, 1.0f);
    m_ButtonToTitle->SetShader("VS_Alpha", "PS_Alpha");
    m_ButtonToTitle->SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_ButtonRetry = AddObject<MouseObject>(instance.GetCamera());
    m_ButtonRetry->SetName("m_ButtonToRetry");
    m_ButtonRetry->SetTexture(textureMgr->GetTexture("Button/Text/Retry.png"));
    m_ButtonRetry->SetPos(0.0f, -200.0f, 0.0f);
    m_ButtonRetry->SetScale(940.0f, 100.0f, 1.0f);
    m_ButtonRetry->SetShader("VS_Alpha", "PS_Alpha");
    m_ButtonRetry->SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    PlayParams corsorParam{};
    corsorParam.volume = DEFAULT_VOLUME;
    m_AudioList.emplace("moveCorsor", AudioConfig(L"SE/MoveCorsor.wav", corsorParam, false, false));

    if (AudioManager* audioMgr = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioMgr->Add(key, config.filePath);
        }
    }

    instance.StopBgm();
    Debug::Log("===== クリアステージ数 : " + std::to_string(m_RelationData.stageCount) + " =====");
}

void ResultScene::Update(float tick)
{
    m_DurationCuror += tick;

    if (Input::GetKeyTrigger(VK_UP) || Input::GetKeyTrigger(VK_DOWN) ||
        Input::GetKeyTrigger(VK_W)  || Input::GetKeyTrigger(VK_S))
    {
        m_isCorsorButtonToTitle ^= true;
        PlaySE("moveCorsor", DEFAULT_VOLUME);
    }
    
    if (m_ButtonToTitle->IsInside()) {
        if (!m_isCorsorButtonToTitle) {
            PlaySE("moveCorsor", DEFAULT_VOLUME);
        }
        m_isCorsorButtonToTitle = true;
    }
    if (m_ButtonRetry->IsInside()) {
        if (m_isCorsorButtonToTitle) {
            PlaySE("moveCorsor", DEFAULT_VOLUME);
        }
        m_isCorsorButtonToTitle = false;
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
    
    bool isMouseLeftTrigger = m_ButtonToTitle->IsDrag() || m_ButtonRetry->IsDrag();
    if (Input::GetKeyTrigger(VK_RETURN) || isMouseLeftTrigger)
    {
        // SEの再生
        PlaySE("enter", DEFAULT_VOLUME);
        Game::SetBgmBpm(100.0f);
        
        if (m_isCorsorButtonToTitle || m_ButtonToTitle->IsDrag())
        {
            // タイトルへ戻る
            SceneTransitionParam transition{ TRANS_MODE::FADE, 0.3f, EASING_TYPE::NONE };
            ChangeScenePush<TitleScene>(ResultToTitle);
        }
        else
        {
            // ゲームを最初から始める
            // シーンに繋ぐ情報は基底初期化後の一番最初に設定
            m_RelationData.isClear = true;
            m_RelationData.stageCount = 0;
            m_RelationData.gameLife   = 4;
            ChangeScenePop(ResultToGame);
        }
    }

    if (m_ButtonToTitle->IsDrag())
    {
        m_isCorsorButtonToTitle = true;
    }
    else if (m_ButtonRetry->IsDrag())
    {
        m_isCorsorButtonToTitle = false;
    }
}

void ResultScene::Draw()
{
    Scene::Draw();
}

void ResultScene::Finalize()
{
    auto& instance = Game::GetInstance();
    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
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
