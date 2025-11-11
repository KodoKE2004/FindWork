#include "SceneList.h"
#include "Game.h"
#include "DebugUI.h"
#include "Cube.h"
#include "Skydome.h"
#include "Square.h"
#include "Model.h"


void TitleScene::Initialize()
{
	auto& instance    = GAME_INSTANCE;
	auto  textureList = GAME_MANAGER_TEXTURE;
    DebugUI::TEXT_CurrentScene = "TitleScene";

#ifdef _DEBUG
	instance.m_Grid.SetEnabled(true);
#endif
	
	// Skydome初期化 
	m_Skydome = instance.AddObject<Skydome>();
	m_Skydome->SetName("m_Skydome");
	m_Skydome->SetSkyDomeMode(true);
	m_Skydome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("SkydomeSpace.png"));
	m_Skydome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_Skydome);
	
	m_TitleLogo = instance.AddObject<Square>();
	m_TitleLogo->SetName("m_TitleLogo");
	m_TitleLogo->SetTexture(textureList->GetTexture("TitleLogo.png"));
	m_TitleLogo->SetScale(800.0f,800.0f,1.0f);
	m_TitleLogo->SetShader("VS_Default","PS_Default");
	m_MySceneObjects.emplace_back(m_TitleLogo);

    
	// オーディオの登録
	m_AudioList.clear();
	PlayParams bgmParam{};
    bgmParam.volume = 0.2f;
    m_AudioList.emplace("bgm",AudioConfig(L"BGM/FaintRain.wav", bgmParam, true, true));

    PlayParams enterParam{};
    m_AudioList.emplace("enter", AudioConfig(L"SE/Enter.wav", enterParam, false, false));

	if (auto audioManager = GAME_MANAGER_AUDIO)
	{
		for (const auto& [key, config] : m_AudioList)
		{
			if (!audioManager->Add(key, config.filePath)) {
				continue;
			}
			if (config.autoPlay)
			{
				auto params = config.params;
				if (config.loop)
				{
					params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
				}
				audioManager->Play(key, params);
			}
		}
	}
}

void TitleScene::Update(float tick)
{
	// Enterの処理
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		// SEの再生
		if (auto audioManager = GAME_MANAGER_AUDIO)
		{
			if (auto it = m_AudioList.find("enter"); it != m_AudioList.end())
			{
				auto params = it->second.params;
				if (it->second.loop)
				{
					params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
				}
				audioManager->Play("enter", params);
			}
			else
			{
				audioManager->Play("enter");
			}

			audioManager->StopAllByName("bgm", false);
		}
		ChangeScenePush<GameSceneHit>(TRANS_MODE::WIPE, 1.0f);
	}

	// Skydomeの回転
	m_Skydome->Spin(0.0f,- 5.0f,0.0f);
}



void TitleScene::Finalize()
{
	auto& instance = GAME_INSTANCE;
#ifdef _DEBUG
	instance.m_Grid.SetEnabled(false);
#endif

	// このシーンのオブジェクトを削除する
	for (auto o : m_MySceneObjects) {
		instance.DeleteObject(o);
	}
	m_MySceneObjects.clear();
    // オーディオの停止
    if (auto audioManager = GAME_MANAGER_AUDIO)
	{
		for (const auto& [key, config] : m_AudioList)
		{
			audioManager->StopAllByName(key);
		}
    }
}

