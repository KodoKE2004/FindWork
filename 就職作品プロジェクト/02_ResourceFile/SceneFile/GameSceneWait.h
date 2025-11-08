#pragma once
#include "Scene.h"

// プライベート定数動的ゲームシーンの遷移用
constexpr int SCENE_SLICE = 0;
constexpr int SCENE_PUSH  = 1;
constexpr int SCENE_HIT   = 2;

class GameSceneWait : public Scene
{
private:

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	GameSceneWait()  = default;
	~GameSceneWait() = default;

	//================================
	// 			ループ内の処理
	//================================

	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()         override;	// シーンの終了処理

	SceneNo GetSceneNo() const override {
		return SceneNo::GAME_WAIT;
	}
};

