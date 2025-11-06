#pragma once
#include "Scene.h"

class GameSceneWait : public Scene
{
private:

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	GameSceneWait() = default;

	SceneNo GetSceneNo() const override {
		return SceneNo::GAME_WAIT;
	}

	~GameSceneWait() = default;

	//================================
	// 			ループ内の処理
	//================================

	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()         override;	// シーンの終了処理

};

