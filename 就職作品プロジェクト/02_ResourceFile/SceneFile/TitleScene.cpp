#include "SceneList.h"
#include "Game.h"
#include "DebugUI.h"
#include "Cube.h"
#include "Skydome.h"
#include "Square.h"
#include "ObjectFile/Model.h"

TitleScene::TitleScene(Camera& cam) : Scene(cam)
{
}

void TitleScene::Initialize()
{
	auto& instance = Game::GetInstance();
	TextureManager* textureMgr = instance;

#ifndef _DEBUG
    DebugUI::TEXT_CurrentScene = "TitleScene";
#endif // !_DEBUG
	// シーンスタックをクリア
    instance.SceneStackClear();
	
	// シーンを取りつなぐデータの作成
	// 失敗判定でWaitシーンに失敗処理をさせないためにtrue
	m_RelationData.stageCount = 0;
	m_RelationData.isClear = true;
	m_RelationData.ClearTransitionTexture();
	m_RelationData.gameLife = 4;

	// タイマー初期化
	m_DurationPressEnter = 0.0f;
	
	// Skydome初期化 
	m_Skydome = AddObject<Skydome>(instance.GetCamera());
	m_Skydome->SetName("m_Skydome");
	m_Skydome->SetSkyDomeMode(true);
	m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
	m_Skydome->SetRadius(5.0f);
	
	m_TitleLogo = AddObject<Square>(instance.GetCamera());
	m_TitleLogo->SetName("m_TitleLogo");
	m_TitleLogo->SetTexture(textureMgr->GetTexture("TitleLogo.png"));
	m_TitleLogo->SetPos  (  0.0f,50.0f,1.0f);
	m_TitleLogo->SetScale(800.0f,800.0f,1.0f);
	m_TitleLogo->SetShader("VS_Default","PS_Default");

	m_PressEnterBack = AddObject<Square>(instance.GetCamera());
	m_PressEnterBack->SetName("m_PressEnterBack");
	m_PressEnterBack->SetTexture(textureMgr->GetTexture("Black.png"));
	m_PressEnterBack->SetPos(0.0f,- 250.0f,0.0f);
	m_PressEnterBack->SetScale(1500.0f,100.0f,1.0f);
	m_PressEnterBack->SetColor(1.0f,1.0f,1.0f,0.4f);

	m_PressEnter = AddObject<Square>(instance.GetCamera());
	m_PressEnter->SetName("m_PressEnter");
	m_PressEnter->SetTexture(textureMgr->GetTexture("Button/Text/PressEnter.png"));
	m_PressEnter->SetPos(0.0f,- 250.0f,0.0f);
	m_PressEnter->SetScale(903.8f,100.0f,1.0f);
	m_PressEnter->SetColor(1.0f,1.0f,1.0f,1.0f);

	if (m_EntryFlg)
	{
		m_FadeMask = AddObject<Square>(instance.GetCamera());
		m_FadeMask->SetScale(1920.0f, 1080.0f, 1.0f);
		m_FadeMask->SetTexture(textureMgr->GetTexture("Black.png"));
		m_FadeMask->SetColor(1.0f,1.0f,1.0f,1.0f);
	}
	
	// オーディオの登録
	m_AudioList.clear();
	PlayParams bgmParam{};
    bgmParam.volume = DEFAULT_VOLUME;
    m_AudioList.emplace("bgm",AudioConfig(L"BGM/TitleScene.wav", bgmParam, true, true));

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

	bool isMouseEnterTrigger = Input::GetMouseTrigger(vkLEFT);
#ifdef _DEBUG
	isMouseEnterTrigger		 = isMouseEnterTrigger && Input::IsMouseInGameView();
#endif
	bool isInsideScreenEnter = (Input::GetMousePos().x > - 640.0f && Input::GetMousePos().x < 640.0f  &&
								Input::GetMousePos().y > - 360.0f && Input::GetMousePos().y < 360.0f) &&
								isMouseEnterTrigger;

	// Enterの処理
	if (Input::GetKeyTrigger(VK_RETURN) ||
		isInsideScreenEnter)
	{
		// SEの再生
		PlaySE("enter", 0.7f);
		Game::SetIsTickCount(false);
		ChangeScenePush<GameSceneWait>(TitleToWait);
		return;
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

}

void TitleScene::Draw()
{
	Scene::Draw();
}

void TitleScene::Finalize()
{
	auto& instance = Game::GetInstance();

	// このシーンのオブジェクトを削除する
	for (auto o : m_MySceneObjects) {
		DeleteObject(o);
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

