#pragma once
#include "Scene.h"
#include "../Texture2D.h"

class GameScene : public Scene
{
// inゲーム基底クラス
protected:

    float m_Tick = 0.0f;				// 経過時間
    float m_SceneChangeTime = 10.0f;	// シーン変更までの時間

private:
	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Texture2D* m_Background = nullptr;



public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	GameScene()  = default;
	~GameScene() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize()		override;
	// シーンの更新
	void Update(float tick) override;
	// シーンの終了処理
	void Finalize()			override;

    void TickCount(const float tick) { m_Tick += tick; }
    bool IsTimeUp() const			 { return m_Tick >= m_SceneChangeTime; }

};

