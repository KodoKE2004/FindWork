#include "Game.h"
#include "Renderer.h"
#include "Application.h"
#include "SceneFile/SceneList.h"
#include "DebugUI.h"
#include "Audio.h" 
#include "SceneFile/Transition/Fade.h"   // フェード型の識別に必要
#include <algorithm>           // 使わないなら不要（2パス版は不要）
#include <cmath>

std::unique_ptr<Game> Game::m_pInstance  = nullptr; // ゲームのインスタンス初期化

Game::Game()
{
	
}

void Game::Initialize()
{	
	auto& instance = GetInstance();

	instance.m_Input			 = std::make_unique<Input>();				// 入力の初期化
	instance.m_Camera			 = std::make_unique<Camera>();				// カメラ作成
    instance.m_TransitionTexture = nullptr;									// トランジション用テクスチャ初期化

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

	Renderer::Start();

#ifdef _DEBUG
	if (!instance.m_Grid.IsInitialized())
	{
		instance.m_Grid.Initialize();
	}
	if (instance.m_Grid.IsEnabled())
	{
		instance.m_Grid.Draw();
	}
#endif	
	for (auto& o : instance.m_GameObjects)
	{
		if (!o) continue;

		o->Draw();
	}
	
	// TransitionTextureを最後に描く（必ず最前面になる）
    auto transTex = instance.m_TransitionTexture;
	if (transTex != nullptr) {
        transTex->Draw();
	}

	DebugUI::Render();
	Renderer::Finish();
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

	// 終了処理
	pt->Finalize(); 

	// 要素を削除
	auto& objs = instance.m_GameObjects;

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
