#pragma once
#include "Scene.h"

class GameSceneWait : public Scene
{
private:

public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	GameSceneWait() = default;
	~GameSceneWait() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize()		override;
	// シーンの更新
	void Update(float tick) override;
	// シーンの終了処理
	void Finalize()         override;
};

