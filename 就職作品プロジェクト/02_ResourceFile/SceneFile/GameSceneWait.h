#pragma once
#include "Scene.h"
#include "Square.h"
#include <vector>
#include <random>

class GameSceneWait : public Scene
{
private:
    uint32_t m_LifeCount = 4;					// 自分のライフ
	std::vector<Square*> m_LifeGame;			// 自身の全ライフ

    SCENE_NO m_SelectedScene = SCENE_NO::NONE; // 選択されたシーン
	bool m_ShouldTransitionToStage = false;
	float m_Tick = 0.0f;
    std::mt19937_64 m_RandomEngine{ std::random_device{}() };

    void PrepareNextStage();

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

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_WAIT;
	}
};

