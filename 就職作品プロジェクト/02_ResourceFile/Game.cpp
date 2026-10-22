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

using namespace Calculator::Easing;

std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化
uint64_t Game::m_DrawFrameCounter = 0;
int	  Game::m_DifficultyStageInterval = 8;
float Game::m_BaseBpmIncreasePerDifficulty = 5.0f;
int   Game::m_SpeedUpStageInterval = 4;
float Game::m_SpeedUpBpmIncrease = 10.0f;
RhythmBeat Game::m_RhythmBeat;
bool  Game::m_isTickCount = false;
bool  Game::s_HasFirstGameSceneWaitInitialized = false;

void Game::InitializeTransitionCSV()
{
    constexpr char kTransitionCsvPath[] = "01_AssetFile/Csv/TransitionData.csv";

	std::string loadError;
	if (!LoadTransitionSettingsFromCsv(kTransitionCsvPath, loadError))
	{
        Debug::Log(loadError, MESSAGE_COLOR::RED);
	}
	else
	{
        Debug::Log("[[成功]] LoadTransitionSettingFromCsv : " + std::string(kTransitionCsvPath),MESSAGE_COLOR::GREEN);
	}
}

void Game::FinalizeTransitionCSV()
{
	std::filesystem::path savePath = SaveDir() / "01_AssetFile/Csv/TransitionData.csv";
	std::string saveError;

	if (!SaveTransitionSettingsToCsv(savePath.string(), saveError))
	{
		Debug::Log(saveError, MESSAGE_COLOR::RED);
	}
	else
	{
		Debug::Log("[[成功]] SaveTransitionSettingFromCsv : " + savePath.string(), MESSAGE_COLOR::GREEN);
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
	
	// 入力/カメラは全シーンで使うため、シーン生成より先に用意する。
	instance.m_Input			 = std::make_unique<Input>();	
	instance.m_Camera			 = std::make_shared<Camera>();	
	//instance.m_Camera->Initialize();							
    instance.m_TransitionTexture.clear();						
    instance.m_Theme             = nullptr;						
	//		シーンをタイトルシーンに設定
	Renderer::Initialize();
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());	// デバッグUIの初期化

#ifdef _DEBUG

	ResistDebugObject();

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

	// Manager 群は Scene より長寿命で、各シーンが共通利用する。
	// 初期化順を固定することで「Scene初期化時に取得できない」状態を防ぐ。
	instance.m_ShaderManager  = std::make_shared<ShaderManager> ("04_ShaderFile/");
	instance.m_TextureManager = std::make_shared<TextureManager>("01_AssetFile/Texture/");
	instance.m_AudioManager   = std::make_shared<AudioManager>	(L"01_AssetFile/Sound/");
	instance.m_AudioManager->Initialize();

	// シェーダー登録
	instance.m_ShaderManager->Add("VS_Default",ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Default",ShaderStage::PS);
	instance.m_ShaderManager->Add("VS_Unlit"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Unlit"  ,ShaderStage::PS);
	instance.m_ShaderManager->Add("VS_Alpha"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Alpha"  ,ShaderStage::PS);
	instance.m_ShaderManager->Add("VS_Instansing2D"  ,ShaderStage::VS);

	// BGM は拍進行の基準にも使うため、Game 側で一元的に保持する。
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

	if (instance.m_BgmAudio)
	{
		instance.m_BgmParams = bgmConfig.params;
		SetBgmBpm(bgmConfig.bpm);
	}

	instance.m_SceneCurrent = std::make_shared<TitleScene>(instance.GetCamera());		// タイトルシーンのインスタンスを生成
	instance.m_SceneCurrent->Initialize();
}

void Game::Update(float tick)
{	
	auto& instance = GetInstance();
	instance.m_Input->Update(Application::GetWindow());

	// 1) Scene 本体更新（ゲーム進行の主処理）
	instance.m_SceneCurrent->Update(tick);

	// 2) カメラ更新（Sceneロジックの結果を反映）
	instance.m_Camera->Update();

	// 3) Scene が所有する Object 更新（見た目/挙動の追従）
	for (auto& o : instance.m_SceneCurrent->GetSceneObjects())
	{
		if (o == nullptr) { continue; }
		o->Update(); // オブジェクトの更新
	}
	if (instance.m_Theme)
	{
		instance.m_Theme->Update();
	}
	// 4) オーディオ反映（Play/Stop 指示を最終的に XAudio へ反映）
	instance.m_AudioManager->Update();

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

	// Draw 順は「Scene本体 → 遷移演出 → テーマUI」の順で固定。
	// これにより遷移演出を常に最前へ重ねられる。
	auto& currentScene = instance.m_SceneCurrent;
	if(currentScene != nullptr)
	{
		currentScene->Draw();
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

	// 破棄は「利用側から順に」実施し、依存先を先に消さない。
	DebugUI::DisposeUI();		// デバッグUIの終了処理
	if (!instance.m_TransitionTexture.empty()) {
		for (auto it : instance.m_TransitionTexture) {
            if (!it) { continue; }
			it->Finalize();
		}
	}
	if(instance.m_Theme){
		instance.m_Theme->Finalize();
	}
	if (instance.m_SceneCurrent)
	{
		instance.m_SceneCurrent->Finalize();
	}
	instance.m_BgmAudio.reset();
	instance.m_SceneCurrent.reset();
	instance.m_SceneList.clear();

	Renderer::Finalize();
}

void Game::SetSceneCurrent(pShared<Scene> newScene)
{
	auto& instance = GetInstance();
	instance.m_SceneCurrent = std::move(newScene);
	if (instance.m_SceneCurrent && instance.m_Theme)
	{
		instance.m_SceneCurrent->SetTheme(instance.m_Theme);
	}
}

void Game::SetSceneNext(pShared<Scene> newScene)
{
	auto& instance = GetInstance();
	instance.m_SceneNext = std::move(newScene);
	if (instance.m_SceneNext && instance.m_Theme)
	{
		instance.m_SceneNext->SetTheme(instance.m_Theme);
	}
}

void Game::SceneStackClear()
{
	auto& instance = GetInstance();
    auto& sceneStack = instance.m_SceneList;

	// 直前シーンへ戻る導線だけ残すため、末尾1件を残して破棄する。
    if (sceneStack.size() <= 1) { return; }
	sceneStack.erase(sceneStack.begin(), sceneStack.end() - 1);
}

void Game::SetTheme(const pShared<Theme>& theme)
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

pShared<Theme> Game::GetTheme()
{
	auto& instance = GetInstance();
	
	// Theme は必要になった瞬間に生成する（起動直後の初期化コストを抑える）。
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

pShared<Scene> Game::GetCurrentScene()
{
	return m_pInstance->m_SceneCurrent;
}

Camera& Game::GetCamera()
{
	if (!m_Camera)
	{
		Debug::Log("[[警告]] Cameraが未生成のため、遅延生成します", MESSAGE_COLOR::YELLOW);
		m_Camera = std::make_unique<Camera>();
	}
	return *m_Camera.get();
}

void Game::SetBgmBpm(float bpm)
{
	if (bpm <= 0.0f) {
		return;
	}

	m_RhythmBeat.SetBpm(bpm);

	auto& instance = Game::GetInstance();
	if (!instance.m_BgmAudio) {
		return;
	}

	instance.m_BgmAudio->SetBpm(bpm);

	const float baseBpm = instance.m_BgmAudio->GetBaseBpm();
	if (baseBpm > 0.0f) {
		instance.m_BgmParams.pitch = bpm / baseBpm;
	}
}

float Game::GetBgmBpm()
{
	auto& instance = Game::GetInstance();
	if (instance.m_BgmAudio) {
		return instance.m_BgmAudio->GetBpm();
	}

	return m_RhythmBeat.GetBpm();
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

void Game::PlayBgm()
{
	auto& instance = Game::GetInstance();
	if (instance.m_BgmAudio) {
		instance.m_BgmAudio->Play(instance.m_BgmParams);
	}
}

void Game::StopBgm()
{
	auto& instance = Game::GetInstance();

	if (instance.m_BgmAudio)
	{
		instance.m_BgmAudio->RequestStopGracefully();
	}
}

void Game::ResistDebugObject()
{
#ifdef _DEBUG
	// ImGui 描画処理を登録
	DebugUI::RedistDebugFunction([]()
		{
			vector<pShared<Object>> objects = GetInstance().GetCurrentScene()->GetSceneObjects();
			
			ImGui::Begin("Game Objects");

			ImGui::Text("Object Count: %zu", objects.size());
			ImGui::Separator();

			static int selectedIndex = -1;

			ImGui::BeginChild("ObjList", ImVec2(220, 0), true);
			for (int i = 0; i < (int)objects.size(); ++i)
			{
				auto& up = objects[i];
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
			if (selectedIndex >= 0 && selectedIndex < (int)objects.size() && objects[selectedIndex])
			{
				Object* obj = objects[selectedIndex].get();
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