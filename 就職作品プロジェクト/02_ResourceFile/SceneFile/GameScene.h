#pragma once
#include "Scene.h"

class GameScene : public Scene
{
public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	GameScene() { Initialize(); }
	~GameScene() { Finalize(); }

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize() override;
	// シーンの更新
	void Update()     override;

	// シーンの終了処理
	void Finalize();
};

