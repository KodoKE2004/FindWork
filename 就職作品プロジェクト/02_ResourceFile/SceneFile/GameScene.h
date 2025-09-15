#pragma once
#include "Scene.h"
#include "../Texture2D.h"

class GameScene : public Scene
{
private:
	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Texture2D* m_Background = nullptr;

	int m_ResultCount = 0;

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
	void Initialize() override;
	// シーンの更新
	void Update(float tick)     override;

	// シーンの終了処理
	void Finalize()   override;
};

