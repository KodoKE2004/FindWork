#pragma once
#include "Scene.h"
#include "Texture2D.h"

class GameSceneExe : public Scene
{
// inゲーム基底クラス
protected:

    float m_Tick = 0.0f;				// 経過時間
    const float m_SceneChangeTime = 5.0f;	// シーン変更までの時間

	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Texture2D* m_Background = nullptr;



public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	GameSceneExe()  = default;

	SceneNo GetSceneNo() const override {
		return SceneNo::GAME_EXE;
	}

	~GameSceneExe() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	virtual void Initialize()		= 0;
	// シーンの更新
	virtual void Update(float tick) = 0;
	// シーンの終了処理
	virtual void Finalize()			= 0;

    void TickCount(const float tick) { m_Tick += tick; }
    bool IsTimeUp() const			 { return m_Tick >= m_SceneChangeTime; }

};

