#pragma once
#include "Scene.h"
#include "Square.h"
#include <vector>
#include <random>

class GameSceneWait : public Scene
{
private:
	// 初期化済みかどうかのフラグ
    // また、乱数選択のリセット用にstaticで持つ
    static bool s_HasFirstGameSceneWaitInitialized;	
	
    uint32_t m_LifeCount = 4;					// 自分のライフ数
    std::vector<Square*> m_LifeGame;			// ライフのオブジェクト格納用

	bool m_ShouldTransitionToStage = false;		// 次のステージを設定できたか判断するフラグ
	float m_Tick = 0.0f;
    std::mt19937_64 m_RandomEngine{ std::random_device{}() };
    bool m_IsFirstInitialized = false;

	// Exeシーンの乱数選択を行う。
	// 二回連続で同じステージが来るようにならないようにする 
	// 要素の削除の仕方は考える。
	void PrepareNextStage();
	SCENE_NO StageSelectAllRandom();
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

	bool IsFirstInitialized() const {
		return m_IsFirstInitialized;
    }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_WAIT;
	}
	void RequestFullStageRandom()
	{
		s_HasFirstGameSceneWaitInitialized = false;
	}
};

