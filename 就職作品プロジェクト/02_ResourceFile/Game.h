#pragma once
#include <memory>
#include "Debug.hpp"
#include "SceneFile/Scene.h"
#include "SceneFile/TransScene.h"
#include "ObjectFile/Object.h"
#include "input.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "AudioManager.h"

class SceneTrans;

class Game
{
private:
	static std::unique_ptr<Game>		 m_pInstance;		// ゲームのインスタンス
	Scene*								 m_SceneCurrent;	// 現在のシーン
	std::unique_ptr<Input>				 m_Input;			// 入力管理
	std::unique_ptr<Camera>				 m_Camera;			// カメラ
	std::vector<std::unique_ptr<Object>> m_GameObjects;		// オブジェクト

	//================================
	//	   ゲームを支えるマネージャー達
	//================================
	std::shared_ptr<MeshManager>		 m_MeshManager;		// シーンで扱うメッシュ
	std::shared_ptr<TextureManager>		 m_TextureManager;	// ゲームで扱う画像
	std::shared_ptr<ShaderManager>		 m_ShaderManager;	// シェーダーをまとめたもの
	std::shared_ptr<AudioManager>		 m_AudioManager;	// オーディオマネージャー

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	Game();
	~Game() = default;
	//================================
	//			ループ内の処理
	//================================
	
	// ゲームの初期化
	static void Initialize();	
	// ゲームの更新
	static void Update();		
	// ゲームの描画
	static void Draw();		
	// ゲームの終了処理
	static void Finalize();
	// 現在のシーンを設定
	static void  SetSceneCurrent(Scene* newScene);

	//===============================
	// ゲッターとシングルトンパターンの実装
	//===============================
	
	// ゲームのインスタンスを取得
	static Game& GetInstance();	
	// 現在のシーンを取得
	Scene* GetCurrentScene() const; 

	Camera* GetCamera() {
		return m_Camera.get();
	}
	
	std::shared_ptr<MeshManager> GetMeshManager() {
		return m_MeshManager;
	}

	std::shared_ptr<TextureManager> GetTextureManager() {
		return m_TextureManager;
	}

	std::shared_ptr<ShaderManager> GetShaderManager() {
		return m_ShaderManager;
	}

	std::shared_ptr<AudioManager> GetAudioManager() {
		return m_AudioManager;
	}
	//================================
	// オブジェクト管理
	//================================
	void DeleteObject(Object* pt); // オブジェクトを削除する
	void DeleteAllObject(); // オブジェクトをすべて削除する

	// オブジェクトを追加する
	template<class T> T* AddObject()
	{
		T* pt = new T(m_Camera.get());
		m_pInstance->m_GameObjects.emplace_back(pt);
		pt->Initialize(); // 初期化
		return pt;
	}

	// オブジェクトを取得する
	template<class T> std::vector<T*> GetObjects()
	{
		std::vector<T*> res;
		for (auto& o : m_pInstance->m_GameObjects) {
			// dynamic_castで型をチェック
			if (T* derivedObj = dynamic_cast<T*>(o.get())) {
				res.emplace_back(derivedObj);
			}
		}
		return res;
	}


};

#define GAME_INSTANCE	Game::GetInstance()
#define GAME_MANAGER_MESH	 GAME_INSTANCE.GetMeshManager()
#define GAME_MANAGER_TEXTURE GAME_INSTANCE.GetTextureManager()
#define GAME_MANAGER_SHADER  GAME_INSTANCE.GetShaderManager()
#define GAME_MANGAER_AUDIO	 GAME_INSTANCE.GetAudioManager()


//================================
//			グローバル関数
// 　　　インスタンスの取得を簡易化
//================================

// シーンを生成するテンプレート関数
template<typename T>
void ChangeScene(TRANS_MODE mode,float duration)
{
	static_assert(std::is_base_of_v<Scene, T>, "T は 基底クラスが Scene ではありません");

	auto scene = new TransScene;
	auto sceneNext = new T;

	scene->SetOldScene(Game::GetInstance().GetCurrentScene());
	scene->SetDuration(duration);
	scene->SetNextScene(sceneNext);
	scene->SetStep(START);
	scene->SetTransMode(mode);
	scene->Initialize();

	Game::GetInstance().SetSceneCurrent(scene);

}
