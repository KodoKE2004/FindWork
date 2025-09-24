#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneFile/SceneList.h"
#include "DebugUI.h"

#include "SceneFile/Transition/Fade.h"   // フェード型の識別に必要
#include <algorithm>           // 使わないなら不要（2パス版は不要）

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
	instance.GetCamera()->Initialize();										// カメラの初期化


	// マネージャーの初期化
	// モデル・テクスチャのパスを設定
	instance.m_MeshManager	  = std::make_shared<MeshManager>	("AssetFile/Model/");
	instance.m_TextureManager = std::make_shared<TextureManager>("AssetFile/Texture/");
	instance.m_ShaderManager  = std::make_shared<ShaderManager> ("ShaderFile/");
	instance.m_AudioManager   = std::make_shared<AudioManager>	(L"AssetFile/Sound/");
	instance.m_AudioManager->Init();

	// シェーダー登録
	instance.m_ShaderManager->AddShader("VS_Default",ShaderStage::VS);
	instance.m_ShaderManager->AddShader("PS_Default",ShaderStage::PS);

	instance.m_ShaderManager->AddShader("VS_Unlit"  ,ShaderStage::VS);
	instance.m_ShaderManager->AddShader("PS_Unlit"  ,ShaderStage::PS);

	instance.m_ShaderManager->AddShader("VS_Alpha"  ,ShaderStage::VS);
	instance.m_ShaderManager->AddShader("PS_Alpha"  ,ShaderStage::PS);

#ifdef _DEBUG

	instance.m_ShaderManager->AddShader("VS_DebugGrid" , ShaderStage::VS);
	instance.m_Grid.Initialize(*instance.m_ShaderManager);

#endif // _DEBUG

	//　モデル登録
	instance.m_MeshManager->AddMeshModel("RedMan" , "Akai.fbx"   , "Akai"   );
	instance.m_MeshManager->AddMeshModel("Pokemon", "Porygon.fbx", "Porygon");
	instance.m_MeshManager->AddMeshModel("aaa"    , "plane.fbx"  , "plane"  );
    instance.m_MeshManager->AddMeshModel("space"  , "space.fbx"  , "Space"  );

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
}

void Game::Draw()
{
	auto& instance = GetInstance();
	Renderer::Start();  // 描画の開始

#ifdef _DEBUG
	if (!instance.m_Grid.IsInitialized())
	{
		instance.m_Grid.Initialize(*instance.m_ShaderManager);
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
	instance.m_Grid.Finalize();
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
