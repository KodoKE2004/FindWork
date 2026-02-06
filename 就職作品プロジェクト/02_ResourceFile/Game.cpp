#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneList.h"
#include "DebugUI.h"
#include "Audio.h" 
#include "Fade.h"
#include "CSVLoader.h"

#include <fstream>
#include <memory>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <typeinfo>

using namespace Math::Easing;

std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化
uint64_t Game::m_DrawFrameCounter = 0;
int	  Game::m_DifficultyStageInterval = 8;
float Game::m_BaseBpmIncreasePerDifficulty = 5.0f;
int   Game::m_SpeedUpStageInterval = 4;
float Game::m_SpeedUpBpmIncrease = 10.0f;

void Game::InitializeTransitionCSV()
{
    constexpr char kTransitionCsvPath[] = "AssetFile/Csv/TransitionData.csv";

	std::string loadError;
	if (!LoadTransitionSettingsFromCsv(kTransitionCsvPath, loadError))
	{
        Debug::Log(loadError, MessageColor::Red);
	}
	else
	{
        Debug::Log("[[成功]] LoadTransitionSettingFromCsv : " + std::string(kTransitionCsvPath),MessageColor::Green);
	}
}

void Game::FinalizeTransitionCSV()
{
	std::filesystem::path savePath = SaveDir() / "AssetFile/Csv/TransitionData.csv";
	std::string saveError;

	if (!SaveTransitionSettingsToCsv(savePath.string(), saveError))
	{
		Debug::Log(saveError, MessageColor::Red);
	}
	else
	{
		Debug::Log("[[成功]] SaveTransitionSettingFromCsv : " + savePath.string(), MessageColor::Green);
	}
}

Game::Game()
{
    m_SceneCurrent = nullptr; // 現在のシーン初期化
    m_SceneNext	   = nullptr; // 次のシーン初期化
}

void Game::Initialize()
{	
	auto& instance = GetInstance();
	
	instance.m_Input			 = std::make_unique<Input>();	
	instance.m_Camera			 = std::make_unique<Camera>();	
	//instance.m_Camera->Initialize();							
    instance.m_TransitionTexture.clear();						
    instance.m_Theme             = nullptr;						
	//		シーンをタイトルシーンに設定
	Renderer::Initialize();
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());	// デバッグUIの初期化

#ifdef _DEBUG

	RegistDebugObject();

	SceneTransitionParam initParam = {
        TRANS_MODE::FADE,
		1.0f,
        EASING_TYPE::IN_OUT_SINE
    };

	TitleToWait   = initParam;
	WaitToGame    = initParam;
	GameToWait    = initParam;
	WaitToResult  = initParam;
	ResultToTitle = initParam;
	ResultToGame  = initParam;

	DebugUI::RedistDebugFunction([]() {
		DrawInGameStateGUI();
	});

#endif
	
	InitializeTransitionCSV();

	// マネージャーの初期化
	// モデル・テクスチャのパスを設定
	instance.m_ShaderManager  = std::make_shared<ShaderManager> ("ShaderFile/");
	instance.m_TextureManager = std::make_shared<TextureManager>("AssetFile/Texture/");
	instance.m_AudioManager   = std::make_shared<AudioManager>	(L"AssetFile/Sound/");
	instance.m_AudioManager->Initialize();

	// シェーダー登録
	instance.m_ShaderManager->Add("VS_Default",ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Default",ShaderStage::PS);
	instance.m_ShaderManager->Add("VS_Unlit"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Unlit"  ,ShaderStage::PS);
	instance.m_ShaderManager->Add("VS_Alpha"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Alpha"  ,ShaderStage::PS);


    AudioConfig bgmConfig{};
    bgmConfig.filePath = L"BGM/GameMelody.wav";
	bgmConfig.autoPlay = false;

	bgmConfig.loop = true;
    bgmConfig.params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
    bgmConfig.params.loop.loopBegin  = 0;
    bgmConfig.params.loop.loopLength = 0;	

	bgmConfig.params.volume = 0.6f;
	bgmConfig.params.pitch  = 1.0f;
    bgmConfig.params.pan	= 0.0f;
	bgmConfig.baseBpm		= 100.0f;
    bgmConfig.bpm			= bgmConfig.baseBpm;

    instance.m_BgmAudio = instance.m_AudioManager->Create(bgmConfig);
    instance.m_BgmPlayParams = bgmConfig.params;

	instance.m_SceneCurrent = std::make_shared<TitleScene>();		// タイトルシーンのインスタンスを生成
	instance.m_SceneCurrent->Initialize();
}

void Game::Update(float tick)
{	
	auto& instance = GetInstance();
	instance.m_Input->Update(Application::GetWindow());

    // BGMの更新
	if (instance.m_BgmAudio)
	{
        SCENE_NO currentSceneNo = instance.GetCurrentScene()->GetSceneNo();
        const auto transScene = std::dynamic_pointer_cast<TransScene>(instance.GetCurrentScene());
        // Exe -> Wait への遷移完了時にBGM再生開始
		bool isReadyPlay = transScene->GetNextSceneNo() == SCENE_NO::GAME_WAIT  &&
						   (transScene->GetOldSceneNo() >= SCENE_NO::GAME_SLICE &&
						    transScene->GetOldSceneNo() <  SCENE_NO::EXE_NUM);
        
		if (isReadyPlay) {

		}

	}

	// 現在のシーンの更新
	instance.m_SceneCurrent->Update(tick);

	// カメラの更新
	instance.m_Camera->Update();

	// 入力の更新
	// オブジェクトの更新
	for (auto& o : instance.m_GameObjects)
	{
		if(o == nullptr){ continue; }
		o->Update(); // オブジェクトの更新
	}
	if (instance.m_Theme)
	{
		instance.m_Theme->Update();
	}
	// オーディオマネージャーの更新
	instance.m_AudioManager->Update();

#ifdef _DEBUG
	GetAudioHelperTickDebugger().Tick(tick);
#endif

	if (instance.m_Input->GetKeyTrigger(VK_Z)) {
        instance.Finalize();
        instance.Initialize();
	}
}

void Game::Draw()
{
	auto& instance = GetInstance();

	++m_DrawFrameCounter;
	Renderer::Start();

	for (auto& o : instance.m_GameObjects)
	{
		if (!o) continue;
		o->Draw();
	}

	if (!instance.m_TransitionTexture.empty()) {
		for (auto it : instance.m_TransitionTexture)
		{
			it->Draw();
		}
	}
	if (instance.m_Theme)
	{
		instance.m_Theme->Draw();
	}

	Renderer::EndGameRender();
	Renderer::ClearBackBuffer();

	auto* gameSrv = Renderer::GetGameRenderSRV();
	if (gameSrv)
	{
	#ifdef _DEBUG
		const float width  = static_cast<float>(Application::GetWidth());
		const float height = static_cast<float>(Application::GetHeight());

		auto srvTex = Renderer::BlitSRVToTexture(gameSrv, 1.0f);
	#endif

		Renderer::BlitSRVToBackbuffer(gameSrv);
		
	#ifdef _DEBUG
		DebugUI::Render(srvTex, ImVec2(width, height));
	#endif
	}
	
	Renderer::Finish();
}

void Game::Finalize()
{
	auto& instance = GetInstance();
	FinalizeTransitionCSV();

	DebugUI::DisposeUI();		// デバッグUIの終了処理
	instance.DeleteAllObject();	//オブジェクトを全て削除
	if (!instance.m_TransitionTexture.empty()) {
		for (auto it : instance.m_TransitionTexture) {
            if (!it) { continue; }
			it->Finalize();
		}
	}
	if(instance.m_Theme){
		instance.m_Theme->Finalize();
	}
	if (instance.m_BgmAudio)
	{
		instance.m_BgmAudio->Stop(true);
		instance.m_BgmAudio.reset();
	}
	Renderer::Finalize();			// レンダラーの終了処理
    instance.m_SceneCurrent->Finalize();
	instance.m_SceneCurrent.reset();
	instance.m_SceneList.clear();
}

void Game::SetSceneCurrent(std::shared_ptr<Scene> newScene)
{
	auto& instance = GetInstance();
	instance.m_SceneCurrent = std::move(newScene);
	if (instance.m_SceneCurrent && instance.m_Theme)
	{
		instance.m_SceneCurrent->SetTheme(instance.m_Theme);
	}
}

void Game::SetSceneNext(std::shared_ptr<Scene> newScene)
{
	auto& instance = GetInstance();
	instance.m_SceneNext = std::move(newScene);
	if (instance.m_SceneNext && instance.m_Theme)
	{
		instance.m_SceneNext->SetTheme(instance.m_Theme);
	}
}

void Game::SetTheme(const std::shared_ptr<Theme>& theme)
{
	auto& instance = GetInstance();
	instance.m_Theme = theme;
	if (instance.m_SceneCurrent) {
		instance.m_SceneCurrent->SetTheme(theme);
	}
	if (instance.m_SceneNext) {
		instance.m_SceneNext->SetTheme(theme);
	}
}

std::shared_ptr<Theme> Game::GetTheme()
{
	auto& instance = GetInstance();
	
	if (!m_Theme) {
		m_Theme = std::make_shared<Theme>(instance.GetCamera());
		m_Theme->Initialize();
		m_Theme->SetName("m_Theme");
	}

	return m_Theme;
}

Game& Game::GetInstance()
{
	if (m_pInstance == nullptr){
		m_pInstance = std::make_unique<Game>();
	}

	return *m_pInstance.get();
}

std::shared_ptr<Scene> Game::GetCurrentScene() const
{
	return m_SceneCurrent;
}

Camera& Game::GetCamera()
{
	return *m_Camera.get();
}

void Game::SetBgmBpm(float bpm)
{
	auto& instance = GetInstance();
	if (!instance.m_BgmAudio)
	{
		return;
	}
	instance.m_BgmAudio->SetBpm(bpm);
	Scene::m_RelationData.rhythmBeat.SyncBpm(instance.m_BgmAudio->GetBpm());
}

float Game::GetBgmBpm()
{
	auto& instance = GetInstance();
	if (!instance.m_BgmAudio)
	{
		return 0.0f;
	}
	return instance.m_BgmAudio->GetBpm();
}

void Game::SetBgmBaseBpm(float bpm)
{
	auto& instance = GetInstance();
	if (!instance.m_BgmAudio)
	{
		return;
	}
	instance.m_BgmAudio->SetBaseBpm(bpm);
	instance.m_BgmAudio->SetBpm(instance.m_BgmAudio->GetBpm());
	Scene::m_RelationData.rhythmBeat.SyncBpm(instance.m_BgmAudio->GetBpm());
}

float Game::GetBgmBaseBpm()
{
	auto& instance = GetInstance();
	if (!instance.m_BgmAudio)
	{
		return 0.0f;
	}

	return instance.m_BgmAudio->GetBaseBpm();
}

void Game::SetDifficultyStageInterval(int interval)
{
	m_DifficultyStageInterval = max(interval, 1);
}

int Game::GetDifficultyStageInterval()
{
	return m_DifficultyStageInterval;
}

void Game::SetBaseBpmIncreasePerDifficulty(float bpmIncrease)
{
	m_BaseBpmIncreasePerDifficulty = max(bpmIncrease, 0.0f);
}

float Game::GetBaseBpmIncreasePerDifficulty()
{
	return m_BaseBpmIncreasePerDifficulty;
}

void Game::SetSpeedUpStageInterval(int interval)
{
	m_SpeedUpStageInterval = max(interval, 1);
}

int Game::GetSpeedUpStageInterval()
{
	return m_SpeedUpStageInterval;
}

void Game::SetSpeedUpBpmIncrease(float bpmIncrease)
{
	m_SpeedUpBpmIncrease = max(bpmIncrease, 0.0f);
}

float Game::GetSpeedUpBpmIncrease()
{
	return m_SpeedUpBpmIncrease;
}

void Game::PlayBgmIfStopped()
{
	if (m_BgmAudio && !m_BgmAudio->IsPlaying())
	{
		m_BgmAudio->Play(m_BgmPlayParams);
	}
}

void Game::StopBgm()
{
	if (m_BgmAudio && m_BgmAudio->IsPlaying())
	{
		m_BgmAudio->Stop();
	}
}

bool Game::IsBgmPlaying() const
{
	return m_BgmAudio && m_BgmAudio->IsPlaying();
}

void Game::RegistDebugObject()
{
#ifdef _DEBUG
	// ImGui 描画処理を登録
	DebugUI::RedistDebugFunction([]()
		{
			auto& instance = Game::GetInstance();

			ImGui::Begin("Game Objects");

			ImGui::Text("Object Count: %zu", instance.m_GameObjects.size());
			ImGui::Separator();

			static int selectedIndex = -1;

			ImGui::BeginChild("ObjList", ImVec2(220, 0), true);
			for (int i = 0; i < (int)instance.m_GameObjects.size(); ++i)
			{
				auto& up = instance.m_GameObjects[i];
				std::string label = up ? up->GetName() : std::string("null");
				if (label.empty()) label = std::string("Object ") + std::to_string(i);
				if (ImGui::Selectable(label.c_str(), selectedIndex == i))
				{
					selectedIndex = i;
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginGroup();
			if (selectedIndex >= 0 && selectedIndex < (int)instance.m_GameObjects.size() && instance.m_GameObjects[selectedIndex])
			{
				Object* obj = instance.m_GameObjects[selectedIndex].get();
				ImGui::Text("Name: %s", obj->GetName().c_str());

				// Position
				NVector3 pos = obj->GetPos();
				float posf[3] = { pos.x, pos.y, pos.z };
				if (ImGui::InputFloat3("Position", posf))
				{
					obj->SetPos(posf[0], posf[1], posf[2]);
				}

				// Rotation
				NVector3 rot = obj->GetRotate();
				float rotf[3] = { rot.x, rot.y, rot.z };
				if (ImGui::InputFloat3("Rotation", rotf))
				{
					obj->SetRotate(rotf[0], rotf[1], rotf[2]);
				}

				// Scale
				NVector3 scl = obj->GetScale();
				float sclf[3] = { scl.x, scl.y, scl.z };
				if (ImGui::InputFloat3("Scale", sclf))
				{
					obj->SetScale(sclf[0], sclf[1], sclf[2]);
				}
			}
			else
			{
				ImGui::Text("No selection");
			}
			ImGui::EndGroup();

			ImGui::End();
		});
#endif // _DEBUG
}

void Game::DeleteObject(const std::shared_ptr<Object>& pt)
{
	auto& instance = GetInstance();
	if (pt == nullptr) return;

	auto& objs = instance.m_GameObjects;
	const auto raw = pt.get();
    auto it = std::find_if(objs.begin(), objs.end(),
		[raw](const std::shared_ptr<Object>& up) {
			return up.get() == raw;
		});

	if (it != objs.end())
	{
		(*it)->Finalize();
		objs.erase(it);
	}

}

void Game::DeleteAllObject()
{
	auto& instance = GetInstance();
	// オブジェクト終了処理
	for (auto& o : m_pInstance->m_GameObjects)
	{
		if (!o) {
			o->Finalize();
		}
	}

	instance.m_GameObjects.clear();
	instance.m_GameObjects.shrink_to_fit();
}
