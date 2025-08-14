#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneFile/SceneList.h"
#include "DebugUI.h"


std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化

Game::Game()
{
	
}

void Game::Initialize()
{	
	auto& instance = GetInstance();

	instance.m_Input	   = std::make_unique<Input> ();
	instance.m_Camera	   = std::make_unique<Camera>();	// カメラ作成
	
	//		シーンをタイトルシーンに設定
	Renderer::Initialize();							// レンダラーの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext()); 		// デバッグUIの初期化
	instance.GetCamera()->Initialize();											// カメラの初期化

	instance.m_GameMeshes	  = std::make_shared<MeshManager>();
	instance.m_TextureManager = std::make_shared<TextureManager>("01_AssetFile/Texture/");
	instance.m_ShaderManager  = std::make_shared<ShaderManager> ("02_ResourceFile/ShaderFile/");
	
	instance.m_ShaderManager->AddShader("VS_Default",ShaderStage::VS);
	instance.m_ShaderManager->AddShader("PS_Default",ShaderStage::PS);

	instance.m_GameMeshes->AddMeshModel("RedMan" , "Akai.fbx"   , "Akai");
	instance.m_GameMeshes->AddMeshModel("Pokemon", "Porygon.fbx", "Porygon");
	instance.m_GameMeshes->AddMeshModel("aaa"    , "plane.fbx"  , "plane"  );

	instance.m_SceneCurrent = new TitleScene;				// タイトルシーンのインスタンスを生成
	instance.m_SceneCurrent->Initialize();
}

void Game::Update()
{	
	auto& instance = GetInstance();
	instance.m_Input->Update();

	// 現在のシーンの更新
	instance.m_SceneCurrent->Update();

	// カメラの更新
	instance.m_Camera->Update();
	// 入力の更新
	// オブジェクトの更新
	for (auto& o : instance.m_GameObjects)
	{
		if(o == nullptr){ continue; }
		o->Update(); // オブジェクトの更新
	}
}

void Game::Draw()
{
	auto& instance = GetInstance();
	Renderer::Start();		// 描画の開始

	// オブジェクト描画
	for (auto& o : instance.m_GameObjects)
	{
		o->Draw();
	}

	DebugUI::Render();			// デバッグUIの描画
	Renderer::Finish();			// 描画の終了
}

void Game::Finalize()
{
	auto& instance = GetInstance();
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
	instance.m_GameMeshes->Clear();
	instance.m_GameObjects.shrink_to_fit();
}
