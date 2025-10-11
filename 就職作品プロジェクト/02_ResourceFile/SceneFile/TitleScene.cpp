#include "SceneList.h"
#include "../Game.h"
#include "../DebugUI.h"
#include "../ObjectFile/Cube.h"
#include "../ObjectFile/Sphere.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"


void TitleScene::Initialize()
{
	auto& instance = GAME_INSTANCE;
    DebugUI::TEXT_CurrentScene = "TitleScene";

#ifdef _DEBUG
	instance.m_Grid.SetEnabled(true);
#endif
	auto* m_SkyDome = instance.AddObject<Sphere>();
	m_SkyDome->SetSkyDomeMode(true);
	m_SkyDome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Plane.png"));
	m_SkyDome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_SkyDome);

    // オーディオの登録
	m_AudioList.clear();
	PlayParams bgmParam;
    bgmParam.volume = 0.2f;
    m_AudioList.emplace("bgm",AudioConfig(L"BGM/FaintRain.wav", bgmParam, true, true));

    PlayParams enterParam{};
    m_AudioList.emplace("enter", AudioConfig(L"SE/Enter.wav", enterParam, false, false));

	if (auto audioManager = Game::GetInstance().GetAudioManager())
	{
		for (const auto& [key, config] : m_AudioList)
		{
			if (!audioManager->Add(key, config.filePath)) {
				continue;
			}

			if (config.autoPlay)
			{
				auto params = config.params;
				// ループが有効なら設定して最初から再生する
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
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		if (auto audioManager = Game::GetInstance().GetAudioManager())
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

		ChangeScene<GameScene>(TRANS_MODE::FADE, 1.0f);
	}




}



void TitleScene::Finalize()
{

#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(false);
#endif
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
    // オーディオの停止
    if (auto audioManager = Game::GetInstance().GetAudioManager())
	{
		for (const auto& [key, config] : m_AudioList)
		{
			audioManager->StopAllByName(key);
		}
    }
}

