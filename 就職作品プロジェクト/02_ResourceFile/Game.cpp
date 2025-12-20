#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneList.h"
#include "DebugUI.h"
#include "Audio.h" 
#include "Fade.h"
#include "CSVLoader.h"

#include <fstream>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <typeinfo>


std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化

void Game::InitializeTransitionCSV()
{
    constexpr char kTransitionCsvPath[] = "AssetFile/Csv/TransitionData.csv";

	std::string loadError;
	if (!LoadTransitionSettingsFromCsv(kTransitionCsvPath, loadError))
	{
        Debug::Log(loadError);
	}
	else
	{
        Debug::Log("[[成功]] LoadTransitionSettingFromCsv : " + std::string(kTransitionCsvPath));
	}
}

void Game::FinalizeTransitionCSV()
{
	std::filesystem::path savePath = SaveDir() / "AssetFile/Csv/TransitionData.csv";
	std::string saveError;

	if (!SaveTransitionSettingsToCsv(savePath.string(), saveError))
	{
		Debug::Log(saveError);
	}
	else
	{
		Debug::Log("[[成功]] SaveTransitionSettingFromCsv : " + savePath.string());
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
	
	instance.m_Input			 = std::make_unique<Input>();			// 入力の初期化
	instance.m_Camera			 = std::make_unique<Camera>();			// カメラ作成
	//instance.m_Camera->Initialize();									// カメラの初期化
    instance.m_TransitionTexture = nullptr;								// トランジション用テクスチャ初期化

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
		DrawTransitionStateGUI();
	});

#endif
	
	InitializeTransitionCSV();

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

	instance.m_SceneCurrent = std::make_shared<TitleScene>();				// タイトルシーンのインスタンスを生成
	instance.m_SceneCurrent->Initialize();
}

void Game::Update(float tick)
{	
	auto& instance = GetInstance();
	

	instance.m_Input->Update(Application::GetWindow());

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

	Renderer::Start();

	for (auto& o : instance.m_GameObjects)
	{
		if (!o) continue;
		o->Draw();
	}
	

	#ifdef _DEBUG
		Renderer::PresentDebugGameView();
	#endif

	// TransitionTextureを最後に描く（必ず最前面になる）
	if (instance.m_TransitionTexture != nullptr) {
	    instance.m_TransitionTexture->Draw();
    }

	DebugUI::Render();
	Renderer::Finish();
}

void Game::Finalize()
{
	auto& instance = GetInstance();
	FinalizeTransitionCSV();

#ifdef _DEBUG
	instance.m_Grid.Finalize();
#endif //_DEBUG

	DebugUI::DisposeUI();		// デバッグUIの終了処理
	instance.DeleteAllObject();	//オブジェクトを全て削除
	Renderer::Finalize();			// レンダラーの終了処理

	instance.m_SceneCurrent.reset();
	instance.m_SceneStack.clear();
}

void Game::SetSceneCurrent(std::shared_ptr<Scene> newScene)
{
	auto& instance = GetInstance();
    instance.m_SceneCurrent = newScene;	// 新しいシーンを設定
	instance.m_SceneCurrent = std::move(newScene);
}

void Game::SetSceneNext(std::shared_ptr<Scene> newScene)
{
	auto& instance = GetInstance();
    instance.m_SceneNext = newScene;
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

void Game::DeleteObject(Object* pt)
{
	auto& instance = GetInstance();
	if (pt == nullptr) return;

	auto& objs = instance.m_GameObjects;
	auto it = std::find_if(objs.begin(), objs.end(),
		[pt](const std::unique_ptr<Object>&up) {
			return up.get() == pt;
		});

	if (it != objs.end())
	{
		(*it)->Finalize();
		objs.erase(it);
		objs.shrink_to_fit();	
	}

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
