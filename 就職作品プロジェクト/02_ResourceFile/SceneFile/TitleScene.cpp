#include "SceneList.h"
#include "Game.h"
#include "DebugUI.h"
#include "Cube.h"
#include "Skydome.h"
#include "Square.h"
#include "Model.h"

TitleScene::TitleScene(bool entryFlg)
{
	m_EntryFlg = entryFlg;
}

void TitleScene::Initialize()
{
	auto& instance    = Game::GetInstance();
	TextureManager* textureMgr = instance;
    DebugUI::TEXT_CurrentScene = "TitleScene";

#ifdef _DEBUG
	instance.m_Grid.SetEnabled(true);
#endif
	// タイマー初期化
	m_DurationPressEnter = 0.0f;
	
	// Skydome初期化 
	m_Skydome = instance.AddObject<Skydome>();
	m_Skydome->SetName("m_Skydome");
	m_Skydome->SetSkyDomeMode(true);
	m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
	m_Skydome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_Skydome);
	
	m_TitleLogo = instance.AddObject<Square>();
	m_TitleLogo->SetName("m_TitleLogo");
	m_TitleLogo->SetTexture(textureMgr->GetTexture("TitleLogo.png"));
	m_TitleLogo->SetPos  (  0.0f,50.0f,1.0f);
	m_TitleLogo->SetScale(800.0f,800.0f,1.0f);
	m_TitleLogo->SetShader("VS_Default","PS_Default");
	m_MySceneObjects.emplace_back(m_TitleLogo);

	m_PressEnter = instance.AddObject<Square>();
	m_PressEnter->SetName("m_PressEnter");
	m_PressEnter->SetTexture(textureMgr->GetTexture("ButtonPressEnter.png"));
	m_PressEnter->SetPos(0.0f,- 250.0f,0.0f);
	m_PressEnter->SetScale(903.8f,100.0f,1.0f);
	m_PressEnter->SetShader("VS_Alpha","PS_Alpha");
	m_PressEnter->SetColor(1.0f,1.0f,1.0f,1.0f);
	m_MySceneObjects.emplace_back(m_PressEnter);

	if (m_EntryFlg)
	{
		m_FadeMask = instance.AddObject<Square>(instance.GetCamera());
		m_FadeMask->SetScale(1920.0f, 1080.0f, 1.0f);
		m_FadeMask->SetTexture(textureMgr->GetTexture("Black.png"));
		m_FadeMask->SetShader("VS_Alpha","PS_Alpha");
		m_FadeMask->SetColor(1.0f,1.0f,1.0f,1.0f);
		m_MySceneObjects.emplace_back(m_FadeMask);
	}
	
	// オーディオの登録
	m_AudioList.clear();
	PlayParams bgmParam{};
    bgmParam.volume = 0.2f;
    m_AudioList.emplace("bgm",AudioConfig(L"BGM/FaintRain.wav", bgmParam, true, true));

    PlayParams enterParam{};
    m_AudioList.emplace("enter", AudioConfig(L"SE/Enter.wav", enterParam, false, false));

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

void TitleScene::Update(float tick)
{
	m_DurationPressEnter += tick;
	m_MoveTitleLogo		 += tick;

	// TitleLogoをふわふわさせる
	// 上下に一定座標を超えると反対方向に一定量移動＆補正
	const float twoPI = 6.27318530718f;
	float phase = (m_MoveTitleLogo / TitleLogoCycle) * twoPI;
	float offsetY = std::sin(phase) * TitleLogoAmp;

	auto pos = m_TitleLogo->GetPos();
	pos.y = TitleLogoBaseY + offsetY;
	m_TitleLogo->SetPos(pos);

	// PressEnterをチカチカさせる
	// 一定時間経過でアルファ値をいじる
	if (m_DurationPressEnter >= AlphaChangeTimer) 
	{
		float alpha = m_PressEnter->GetColor().w;
		m_PressEnter->SetColor(1.0f,1.0f,1.0f,1.0f - alpha);
		m_DurationPressEnter = 0.0f;
	}



	// Enterの処理
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
		ChangeScenePush<GameSceneWait>(TRANS_MODE::WIPE, 1.0f);
	}

	// Skydomeの回転
	m_Skydome->Spin(0.0f, -4.0f, 0.0f);

	//===============================
	//		 開幕のみ流れる処理
	//	下に書くと繰り返すとき反映されません	
	//===============================
	if (!m_EntryFlg || !m_FadeMask)
	{
		return;
	}

	Color colorFadeMask = m_FadeMask->GetColor();
	float fadeSpeed = 1.0f / m_FadeTimer;
	colorFadeMask.w -= (fadeSpeed * tick);

	if (colorFadeMask.w <= 0.0f)
	{
		colorFadeMask.w = 0.0f;
		m_EntryFlg = false;
	}

	m_FadeMask->SetColor(colorFadeMask);


}



void TitleScene::Finalize()
{
	auto& instance = Game::GetInstance();
#ifdef _DEBUG
	instance.m_Grid.SetEnabled(false);
#endif

	// このシーンのオブジェクトを削除する
	for (auto o : m_MySceneObjects) {
		instance.DeleteObject(o);
	}
	m_MySceneObjects.clear();
    // オーディオの停止
    if (AudioManager* audioMgr = instance)
	{
		for (const auto& [key, config] : m_AudioList)
		{
			audioMgr->StopAllByName(key);
		}
    }
}

