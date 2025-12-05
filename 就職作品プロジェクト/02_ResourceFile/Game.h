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
	Scene*								 m_SceneCurrent;	  // 現在のシーン
	std::unique_ptr<Input>				 m_Input;			  // 入力管理
	std::unique_ptr<Camera>				 m_Camera;			  // カメラ
	std::vector<std::unique_ptr<Object>> m_GameObjects;		  // オブジェクト

    std::shared_ptr<TransitionBase>		 m_TransitionTexture; // トランジション用テクスチャ
    std::vector<std::unique_ptr<Scene>>	 m_SceneStack;		  // シーンスタック

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
	
	static void Initialize();		// ゲームの初期化
	static void Update(float tick);	// ゲームの更新
	static void Draw();				// ゲームの描画
	static void Finalize();			// ゲームの終了処理

	// 現在のシーンを設定
	static void  SetSceneCurrent(Scene* newScene);

    // TranstionTextureをTransSceneと連携させる
	void SetTransitionTexture(std::shared_ptr<TransitionBase> tex) {
		m_TransitionTexture = tex;
    }

	std::shared_ptr<TransitionBase> GetTransitionTexture() const {
		return m_TransitionTexture;
    }
    //===============================
	//		シーンのスタック管理
	//===============================
    
	void ScenePush(Scene* newScene);
	Scene* ScenePop();
	size_t GetSceneStackSize() const {
		return m_SceneStack.size();
    }

	//===============================
	// ゲッターとシングルトンパターンの実装
	//===============================
	
	// ゲームのインスタンスを取得
	static Game& GetInstance();	
	// 現在のシーンを取得
	Scene* GetCurrentScene() const; 

	// インスタンスのカメラ
	Camera* GetCamera() {
		return m_Camera.get();
	}
	
	// メッシュマネージャー
	[[nodiscard]] operator MeshManager*   () const { return m_MeshManager.get(); }
	[[nodiscard]] operator TextureManager*() const { return m_TextureManager.get(); }
	[[nodiscard]] operator ShaderManager* () const { return m_ShaderManager.get(); }
	[[nodiscard]] operator AudioManager*  () const { return m_AudioManager.get(); }


	// Debug関連
#ifdef _DEBUG
	DebugGridLine m_Grid;
	static void RegistDebugObject();
#else
	static void RegistDebugFunction(){}
#endif // _DEBUG
	//================================
	// オブジェクト管理
	//================================
	void DeleteObject(Object* pt); // オブジェクトを削除する
	void DeleteAllObject(); // オブジェクトをすべて削除する

	// オブジェクトを追加する
	template<class T, class... Args>
	T* AddObject(Args&&... args)
	{
		static_assert(std::is_base_of_v<Object, T>, "TがObjectを継承していない");
		static_assert(!std::is_abstract_v<T>	  , "Tが抽象クラスだった");

		// コンストラクタ引数を完全転送して unique_ptrを作成
		std::unique_ptr<T> up;
		if constexpr (sizeof...(Args) == 0) {
			up = std::make_unique<T>(m_Camera.get());
		}
		else {
			up = std::make_unique<T>(std::forward<Args>(args)...);
		}
		T* pt = up.get();

		m_pInstance->m_GameObjects.emplace_back(std::move(up));
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

//================================
//			グローバル関数
// 　　　インスタンスの取得を簡易化
//================================

//================================
//	  シーンを遷移するテンプレート関数
//================================
template<class T>
void ChangeScenePush(SceneTransitionParam& state)
{
    auto& instance = Game::GetInstance();

	// テンプレートなので
	// 既定がSceneでなければエラー
	// T が Scene を継承していること、かつ抽象クラスでないことをチェック
	static_assert(std::is_base_of_v<Scene, T>, "T は Scene を継承している必要があります");
	static_assert(!std::is_abstract_v<T>, "T は抽象クラスではいけません");

	MyDebugLog(std::cout << "ChangeScenePushの読み込み検出\n");

	auto scene = new TransScene;
	auto sceneNext = new T;

    // シーン間の受け渡しデータを設定
	SceneRelationData relationData{};
	if (auto currentScene = instance.GetCurrentScene())
	{
		relationData = currentScene->GetRelationData();
		relationData.previousScene = currentScene->GetSceneNo();
	}
	relationData.nextScene = sceneNext->GetSceneNo();

	sceneNext->SetRelationData(relationData);
	scene->SetRelationData(relationData);

    instance.ScenePush(instance.GetCurrentScene());
	scene->SetOldScene(instance.GetCurrentScene());

	scene->SetTransitionParam(state);
	scene->SetNextScene(sceneNext);
	scene->SetStep(STEP::START);
	scene->Initialize();

	instance.SetSceneCurrent(scene);

}

// 一つ前のシーンに戻る
inline void ChangeScenePop(SceneTransitionParam& state)
{
    auto& instance = Game::GetInstance();

	if (instance.GetSceneStackSize() == 0) {
		MyDebugLog(Debug::Log("シーンスタックが空です");)
		return ;
	}

    // 現在のシーン
    auto scene = new TransScene;
    Scene* sceneNext = instance.ScenePop();
	if (sceneNext == nullptr) {
		delete scene;
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

inline void Game::ScenePush(Scene* newScene)
{
    // 遷移前のシーンをスタックに保存
    if (newScene) m_SceneStack.push_back(std::unique_ptr<Scene>(newScene));
}

inline Scene* Game::ScenePop()
{
	if(m_SceneStack.empty())	return nullptr;
	
    Scene* scene = m_SceneStack.back().release();
    m_SceneStack.pop_back();

	return scene;
}

