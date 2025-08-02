#pragma once
#ifdef _DEBUG
#include "Debug.hpp"
#endif

#include <memory>
#include "SceneFile/Scene.h"
#include "ObjectFile/Object.h"
#include "input.h"
#include "MeshManager.h"

class Game
{
private:
	static std::unique_ptr<Game>  m_pInstance;		// ゲームのインスタンス
	Scene*						  m_SceneCurrent;	// 現在のシーン
	std::unique_ptr<Input>		  m_Input;			// 入力管理
	std::unique_ptr<Camera>		  m_Camera;			// カメラ
	std::vector<std::unique_ptr<Object>> m_Objects; // オブジェクト
	std::shared_ptr<MeshManager>  m_GameMeshes;		// シーンで扱うメッシュ

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

	std::unique_ptr<Camera> GetCamera() {
		return std::move(m_Camera);
	}
	
	std::shared_ptr<MeshManager> GetMeshManager() {
		return m_GameMeshes;
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
		m_pInstance->m_Objects.emplace_back(pt);
		pt->Initialize(); // 初期化
		return pt;
	}

	// オブジェクトを取得する
	template<class T> std::vector<T*> GetObjects()
	{
		std::vector<T*> res;
		for (auto& o : m_pInstance->m_Objects) {
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

// シーンを生成するテンプレート関数
template<typename T, typename... Args>
void ChangeScene(Args&&... args)
{
	static_assert(std::is_base_of_v<Scene, T>, "T は 基底クラスが Scene ではありません");

	// 現在のシーンを新しいシーンに変更
	auto scene = new T (std::forward<Args>(args)...);
	Game::GetInstance().SetSceneCurrent(scene);


}