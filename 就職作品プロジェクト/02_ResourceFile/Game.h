#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <Debug.hpp>
#include "DebugGridLine.h"

#include "Scene.h"
#include "TransScene.h"
#include "TransitionBase.h"
#include "Object.h"

#include "input.h"
#include "GameSceneExe.h"

#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "AudioManager.h"


class Game
{
private:
	static std::unique_ptr<Game>		 m_pInstance;		  // ゲームのインスタンス
	std::shared_ptr<Scene>				 m_SceneCurrent;	  // 現在のシーン
    std::shared_ptr<Scene>				 m_SceneNext;		  // 次のシーン
	std::unique_ptr<Input>				 m_Input;			  // 入力管理
	std::unique_ptr<Camera>				 m_Camera;			  // カメラ
	std::vector<std::unique_ptr<Object>> m_GameObjects;		  // オブジェクト

    std::shared_ptr<TransitionBase>		 m_TransitionTexture; // トランジション用テクスチャ
    std::vector<std::shared_ptr<Scene>>	 m_SceneStack;		  // シーンスタック

	//--------------------------------------
	//	   ゲームを支えるマネージャー達
	//--------------------------------------
	std::shared_ptr<MeshManager>		 m_MeshManager;		// シーンで扱うメッシュ
	std::shared_ptr<TextureManager>		 m_TextureManager;	// ゲームで扱う画像
	std::shared_ptr<ShaderManager>		 m_ShaderManager;	// シェーダーをまとめたもの
	std::shared_ptr<AudioManager>		 m_AudioManager;	// オーディオマネージャー

    static void InitializeTransitionCSV();					// トランジションCSVの初期化
    static void FinalizeTransitionCSV();					// トランジションCSVの終了処理

public:
	//--------------------------------------
	//		コンストラクタとデストラクタ
	//--------------------------------------

	Game();
	~Game() = default;
	//--------------------------------------
	//			ループ内の処理
	//--------------------------------------

	static void Initialize();		// ゲームの初期化
	static void Update(float tick);	// ゲームの更新
	static void Draw();				// ゲームの描画
	static void Finalize();			// ゲームの終了処理

	// 現在のシーンを設定
    static void SetSceneCurrent(std::shared_ptr<Scene> newScene);
    static void SetSceneNext(std::shared_ptr<Scene> newScene);
    // TranstionTextureをTransSceneと連携させる
	void SetTransitionTexture(std::shared_ptr<TransitionBase> tex) {
		m_TransitionTexture = tex;
    }

	std::shared_ptr<TransitionBase> GetTransitionTexture() const {
		return m_TransitionTexture;
    }
	//--------------------------------------
	//		シーンのスタック管理
	//--------------------------------------

	void ScenePush(std::shared_ptr<Scene> newScene);
	std::shared_ptr<Scene> ScenePop();
	size_t GetSceneStackSize() const {
		return m_SceneStack.size();
    }

	//--------------------------------------
	// ゲッターとシングルトンパターンの実装
	//--------------------------------------

	// ゲームのインスタンスを取得
	static Game& GetInstance();	
	// 現在のシーンを取得
	std::shared_ptr<Scene> GetCurrentScene() const;

	// インスタンスのカメラ
	Camera& GetCamera() {
		return *m_Camera.get();
	}
	
	// メッシュマネージャー
	[[nodiscard]] operator MeshManager*    () const { return m_MeshManager.get(); }
	[[nodiscard]] operator TextureManager* () const { return m_TextureManager.get(); }
	[[nodiscard]] operator ShaderManager*  () const { return m_ShaderManager.get(); }
	[[nodiscard]] operator AudioManager*   () const { return m_AudioManager.get(); }


	// Debug関連
	DebugGridLine m_Grid;
	static void RegistDebugObject();
	static void RegistDebugFunction(){}

	//--------------------------------------
	//			 オブジェクト管理
	//--------------------------------------
	void DeleteObject(Object* pt); // オブジェクトを削除する
	void DeleteAllObject();		   // オブジェクトをすべて削除する

	// オブジェクトを追加する
	template<class T, class... Args>
	T* AddObject(Args&&... args)
	{
		static_assert(std::is_base_of_v<Object, T>, "TがObjectを継承していない");
		static_assert(!std::is_abstract_v<T>	  , "Tが抽象クラスだった");

		auto& instance = *m_pInstance;

		// コンストラクタ引数を完全転送して unique_ptrを作成
		std::unique_ptr<T> up;

		if constexpr (sizeof...(Args) == 0) {
			up = std::make_unique<T>(*instance.m_Camera.get());
		}
		else {
			up = std::make_unique<T>(std::forward<Args>(args)...);
		}
		T* pt = up.get();

		instance.m_GameObjects.emplace_back(std::move(up));
		pt->Initialize(); // 初期化
		return pt;
	}


	// オブジェクトを取得する
	template<class T> 
	std::vector<T*> GetObjects()
	{
		std::vector<T*> res;
		for (const auto& o : m_pInstance->m_GameObjects) {
			// dynamic_castで型をチェック
			if (T* derivedObj = dynamic_cast<T*>(o.get())) {
				res.emplace_back(derivedObj);
			}
		}
		return res;
	}


};

//--------------------------------------
//			グローバル関数
// 　　　インスタンスの取得を簡易化
//--------------------------------------

//--------------------------------------
//	    シーンを遷移するテンプレート関数
//--------------------------------------
template<class T>
void ChangeScenePush(SceneTransitionParam& state)
{
    auto& instance = Game::GetInstance();

	// テンプレートなので
	// 既定がSceneでなければエラー
	// T が Scene を継承していること、かつ抽象クラスでないことをチェック
	static_assert(std::is_base_of_v<Scene, T>, "T は Scene を継承している必要があります");
	static_assert(!std::is_abstract_v<T>, "T は抽象クラスではいけません");

	Debug::Log("[[検出]] シーンのPush");

	Scene::ClearTimerList();

	auto scene     = std::make_shared<TransScene>();
	auto sceneNext = std::make_shared<T>();
    auto sceneCurrent = instance.GetCurrentScene();

    instance.ScenePush(sceneCurrent);
    instance.SetSceneNext(sceneNext);

	scene->SetOldScene(sceneCurrent);
	scene->SetNextScene(sceneNext);
	scene->SetTransitionParam(state);
	scene->SetStep(STEP::START);
	scene->Initialize();

	instance.SetSceneCurrent(scene);

}

// 一つ前のシーンに戻る
inline void ChangeScenePop(SceneTransitionParam& state)
{
    auto& instance = Game::GetInstance();

	if (instance.GetSceneStackSize() == 0) {
		Debug::Log("[[警告]] シーンスタックが空です");
		return ;
	}

	Debug::Log("[[検出]] シーンのPop");

	Scene::ClearTimerList();

    // 現在のシーン
    auto scene = std::make_shared<TransScene>();
    auto sceneNext = instance.ScenePop();

	if (!sceneNext) {
		return;
	}

	scene->SetOldScene(instance.GetCurrentScene());
    scene->SetRelationData(instance.GetCurrentScene()->GetRelationData());
    scene->SetNextScene(sceneNext);
	scene->SetTransitionParam(state);
	scene->SetStep(STEP::START);
	scene->Initialize();

    instance.SetSceneCurrent(scene);
}

inline void Game::ScenePush(std::shared_ptr<Scene> newScene)
{
    // 遷移前のシーンをスタックに保存
    if (newScene) m_SceneStack.push_back(newScene);
}

inline std::shared_ptr<Scene> Game::ScenePop()
{
	if(m_SceneStack.empty())	return nullptr;
	
    auto scene = m_SceneStack.back();
    m_SceneStack.pop_back();

	return scene;
}

