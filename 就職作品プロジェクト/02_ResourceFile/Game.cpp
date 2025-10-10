#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneFile/SceneList.h"
#include "DebugUI.h"
#include "Audio.h" 
#include "SceneFile/Transition/Fade.h"   // フェード型の識別に必要
#include <algorithm>           // 使わないなら不要（2パス版は不要）
#include <cmath>

#ifdef _DEBUG
#include "../03_ExternalFile/imgui/imgui.h"
#endif

#ifdef _DEBUG
namespace
{
	class AudioHelperTickDebugger
	{
	public:
		AudioHelperTickDebugger()
		{
			m_state.Fs = 48000.0;
			m_state.volDb = LinToDb(0.2f);
			m_state.pan = 0.0f;
			m_state.pitchSemi = 0.0f;
		}

		void Tick(float dt)
		{
			if (dt <= 0.0f) { return; }

			m_elapsed += dt;

			const float targetVolDb = std::sin(m_elapsed * 0.5f) * -6.0f;
			const float targetPitchSem = std::sin(m_elapsed * 0.25f) * 2.0f;
			const float targetPan = std::sin(m_elapsed * 0.15f);

			m_slope.volDbPerSec = SlopeToHitInFrames(m_state.volDb, targetVolDb, 45);
			m_slope.pitchSemiPerSec = SlopeToHitInFrames(m_state.pitchSemi, targetPitchSem, 45);
			m_slope.panPerSec = SlopeToHitInFrames(m_state.pan, targetPan, 45);

			m_waveL.clear();
			m_waveR.clear();
			RenderFrameSlope(m_state, m_slope, dt, m_waveL, m_waveR, 440.0);
		}

		void DrawUI()
		{
			if (ImGui::Begin("Audio Helper Debug"))
			{
				ImGui::Text("Elapsed : %.3f s", m_elapsed);
				ImGui::Text("Volume  : %.2f dB", m_state.volDb);
				ImGui::Text("Pitch   : %.2f semi", m_state.pitchSemi);
				ImGui::Text("Pan     : %.2f", m_state.pan);

				if (!m_waveL.empty())
				{
					ImGui::Separator();
					ImGui::Text("Left Channel");
					ImGui::PlotLines("", m_waveL.data(), static_cast<int>(m_waveL.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
				}
				if (!m_waveR.empty())
				{
					ImGui::Separator();
					ImGui::Text("Right Channel");
					ImGui::PlotLines(" ", m_waveR.data(), static_cast<int>(m_waveR.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
				}
			}
			ImGui::End();
		}

	private:
		VoiceState m_state{};
		SlopePerSec m_slope{};
		float m_elapsed = 0.0f;
		std::vector<float> m_waveL;
		std::vector<float> m_waveR;
	};

	AudioHelperTickDebugger& GetAudioHelperTickDebugger()
	{
		static AudioHelperTickDebugger instance;
		return instance;
	}

	void RegisterAudioHelperDebugUI()
	{
		static bool registered = false;
		if (registered) { return; }
		registered = true;
		DebugUI::RedistDebugFunction([]() { GetAudioHelperTickDebugger().DrawUI(); });
	}
}
#endif // _DEBUG
std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化

Game::Game()
{
	
}

void Game::Initialize()
{	
	auto& instance = GetInstance();

	instance.m_Input	   = std::make_unique<Input>();		// 入力の初期化
	instance.m_Camera	   = std::make_unique<Camera>();	// カメラ作成
	
	//		シーンをタイトルシーンに設定
	Renderer::Initialize();													// レンダラーの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());		// デバッグUIの初期化
#ifdef _DEBUG
	RegisterAudioHelperDebugUI();
#endif
	instance.GetCamera()->Initialize();										// カメラの初期化


	// マネージャーの初期化
	// モデル・テクスチャのパスを設定
	instance.m_ShaderManager  = std::make_shared<ShaderManager> ("ShaderFile/");
	instance.m_MeshManager	  = std::make_shared<MeshManager>	("AssetFile/Model/");
	instance.m_TextureManager = std::make_shared<TextureManager>("AssetFile/Texture/");
	instance.m_AudioManager   = std::make_shared<AudioManager>	(L"AssetFile/Sound/");
	instance.m_AudioManager->Init();

	// シェーダー登録
	instance.m_ShaderManager->Add("VS_Default",ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Default",ShaderStage::PS);

	instance.m_ShaderManager->Add("VS_Unlit"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Unlit"  ,ShaderStage::PS);

	instance.m_ShaderManager->Add("VS_Alpha"  ,ShaderStage::VS);
	instance.m_ShaderManager->Add("PS_Alpha"  ,ShaderStage::PS);

#ifdef _DEBUG

	instance.m_ShaderManager->Add("VS_DebugGrid" , ShaderStage::VS);
	instance.m_Grid.Initialize();

#endif // _DEBUG

	//　モデル登録
	instance.m_MeshManager->Add("RedMan" , "Akai.fbx"   , "Akai"   );
	instance.m_MeshManager->Add("Pokemon", "Porygon.fbx", "Porygon");
	instance.m_MeshManager->Add("null"   , "plane.fbx"  , "plane"  );

    instance.m_MeshManager->Add("space"  , "space.fbx"  , "Space"  );

	instance.m_SceneCurrent = new TitleScene;				// タイトルシーンのインスタンスを生成
	instance.m_SceneCurrent->Initialize();
}

void Game::Update(float tick)
{	
	auto& instance = GetInstance();

	instance.m_Input->Update();

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
	// オーディオマネージャーの更新
	instance.m_AudioManager->Update();
#ifdef _DEBUG
	GetAudioHelperTickDebugger().Tick(tick);
#endif
}

void Game::Draw()
{
	auto& instance = GetInstance();
	Renderer::Start();  // 描画の開始

#ifdef _DEBUG
	if (!instance.m_Grid.IsInitialized())
	{
		instance.m_Grid.Initialize();
	}
	if (instance.m_Grid.IsEnabled())
	{
		instance.m_Grid.Draw();
	}
#endif	// フェード以外を描く

	for (auto& o : instance.m_GameObjects)
	{
		if (!o) continue;
		if (dynamic_cast<Fade*>(o.get())) continue;  // フェードは後で
		o->Draw();
	}
	
	// フェードを最後に描く（必ず最前面になる）
	for (auto& o : instance.m_GameObjects)
	{
		if (!o) continue;
		if (auto* f = dynamic_cast<Fade*>(o.get()))
		{
			f->Draw();
		}
	}

	DebugUI::Render();   // デバッグUIの描画
	Renderer::Finish();  // 描画の終了
}

void Game::Finalize()
{
	auto& instance = GetInstance();
#ifdef _DEBUG
	instance.m_Grid.Finalize();
#endif //_DEBUG
	DebugUI::DisposeUI();		// デバッグUIの終了処理
	instance.DeleteAllObject();	//オブジェクトを全て削除
	Renderer::Finalize();			// レンダラーの終了処理
}

void Game::SetSceneCurrent(Scene* newScene)
{
	auto& instance = GetInstance();
	instance.m_SceneCurrent = newScene;	// 新しいシーンを設定

}

Game& Game::GetInstance()
{
	if (m_pInstance == nullptr){
		m_pInstance = std::make_unique<Game>();
	}

	return *m_pInstance.get();
}

Scene* Game::GetCurrentScene() const
{
	return m_SceneCurrent;
}

void Game::DeleteObject(Object* pt)
{
	auto& instance = GetInstance();
	if (pt == NULL) return;

	pt->Finalize(); // 終了処理

	// 要素を削除
	instance.m_GameObjects.erase(
		std::remove_if(
			instance.m_GameObjects.begin(),
			instance.m_GameObjects.end(),
			[pt](const std::unique_ptr<Object>& element) {return element.get() == pt; }),
			instance.m_GameObjects.end());

	instance.m_GameObjects.shrink_to_fit();
}

void Game::DeleteAllObject()
{
	auto& instance = GetInstance();
	// オブジェクト終了処理
	for (auto& o : m_pInstance->m_GameObjects)
	{
		o->Finalize();
	}

	instance.m_GameObjects.clear();
	instance.m_MeshManager->Clear();
	instance.m_GameObjects.shrink_to_fit();
}
