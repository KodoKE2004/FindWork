#ifdef _DEBUG
#include "Debug.hpp"
#endif 

#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneFile/SceneList.h"
#include "DebugUI.h"


std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化

Game::Game()
{
	m_Input		 = std::make_unique<Input>();
	m_Camera	 = std::make_unique<Camera>();		// カメラ作成
	m_SceneTrans = std::make_unique<SceneTrans>();	// 遷移演出作成
}

void Game::Initialize()
{
	m_pInstance = std::make_unique<Game>();
	//		シーンをタイトルシーンに設定
	Renderer::Initialize();							// レンダラーの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext()); 			// デバッグUIの初期化
	m_pInstance->GetCamera()->Initialize();										// カメラの初期化
	m_pInstance->GetSceneTrans()->Initialize(FADE);

	m_pInstance->m_GameMeshes = std::make_shared<MeshManager>();
	
	
	m_pInstance->m_GameMeshes->AddMeshModel("RedMan", "Akai.fbx", "Akai");
	m_pInstance->m_GameMeshes->AddMeshModel("Pokemon", "Porygon.fbx","Porygon");
	m_pInstance->m_GameMeshes->AddMeshModel("aaa", "plane.fbx","plane");

	m_pInstance->m_SceneCurrent = new TitleScene;	// タイトルシーンのインスタンスを生成
	
}

void Game::Update()
{	
	// 遷移演出の更新　OFFの時は動かない
	m_pInstance->m_SceneTrans->Update();

	if (m_pInstance->m_SceneTrans->IsTransition() == SWITCH::OFF) {
		// 現在のシーンの更新
		m_pInstance->m_SceneCurrent->Update();
	}
	// カメラの更新
	m_pInstance->m_Camera->Update(); 
	// 入力の更新
	m_pInstance->m_Input->Update(); 
	// オブジェクトの更新
	for (auto& o : m_pInstance->m_Objects)
	{
		if(o == nullptr){ continue; }
		o->Update(); // オブジェクトの更新
	}
}

void Game::Draw()
{
	Renderer::Start();		// 描画の開始

	// オブジェクト描画
	for (auto& o : m_pInstance->m_Objects)
	{
		o->Draw();
	}
	m_pInstance->m_SceneTrans->Draw();
	DebugUI::Render();			// デバッグUIの描画
	Renderer::Finish();			// 描画の終了
}

void Game::Finalize()
{
	DebugUI::DisposeUI();		// デバッグUIの終了処理
	m_pInstance->DeleteAllObject();	//オブジェクトを全て削除
	Renderer::Finalize();			// レンダラーの終了処理
}

void Game::SetSceneCurrent(Scene* newScene)
{
	if (m_pInstance->m_SceneCurrent != nullptr) {
		m_pInstance->m_SceneCurrent->Finalize();
		delete m_pInstance->m_SceneCurrent; // 現在のシーンを終了
	}

	m_pInstance->m_SceneCurrent = newScene;	// 新しいシーンを設定

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
	if (pt == NULL) return;

	pt->Finalize(); // 終了処理

	// 要素を削除
	m_pInstance->m_Objects.erase(
		std::remove_if(
			m_pInstance->m_Objects.begin(),
			m_pInstance->m_Objects.end(),
			[pt](const std::unique_ptr<Object>& element) {return element.get() == pt; }),
		m_pInstance->m_Objects.end());
	m_pInstance->m_Objects.shrink_to_fit();
}

void Game::DeleteAllObject()
{
	// オブジェクト終了処理
	for (auto& o : m_pInstance->m_Objects)
	{
		o->Finalize();
	}

	m_pInstance->m_Objects.clear();			
	m_pInstance->m_GameMeshes->Clear();		
	m_pInstance->m_Objects.shrink_to_fit();
}
