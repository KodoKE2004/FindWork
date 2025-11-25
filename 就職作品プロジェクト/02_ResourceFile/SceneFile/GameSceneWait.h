#pragma once
#include "Scene.h"
#include "Square.h"
#include <vector>
#include <random>

class GameSceneWait : public Scene
{
private:
	
    uint32_t m_LifeCount = 4;					// 自分のライフ数
    std::vector<Square*> m_LifeGame;			// ライフのオブジェクト格納用

    // 経過時間管理用変数
	float m_Tick = 0.0f;
    
	std::mt19937_64 m_RandomEngine{ std::random_device{}() };

	//================================
    // 	  シーン内で使う時間関連変数
	//	  (スピード倍率によって変化)
	//================================
    TimerData m_DecrementLife = { 0.0f,0.5f };	// ライフが減るまでのタイマー管理用構造体
    TimerData m_ChangeStage   = { 0.0f,2.0f };	// ステージ遷移までのタイマー管理用構造体

    //================================
    //		 ステージ遷移用フラグ
    //================================ 
	// 初期化済みかどうかのフラグ
    // また、乱数選択のリセット用にstaticで持つ
    static bool s_HasFirstGameSceneWaitInitialized;	

	bool m_ShouldTransitionToStage = false;		// 次のステージを設定できたか判断するフラグ
    bool m_IsFirstInitialized	   = false;		// シーンが最初に初期化されたかどうかのフラグ
    bool m_isStageCleared          = false;		// ステージクリアしたかどうかのフラグ
    bool m_wasDecrementLife		   = false;		// ライフが減ったかどうかのフラグ

    bool m_WasPlayBGM = false;				// BGMを再生したかどうかのフラグ

    RhythmBeat m_RhythmBeat;			// リズムビート管理用変数
    bool m_IsLifeTiltPositive = true;	// ライフの傾きが正かどうかのフラグ

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

	void DecrementLife();
};

