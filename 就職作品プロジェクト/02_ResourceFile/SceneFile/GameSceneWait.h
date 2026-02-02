#pragma once
#include "Scene.h"
#include "Square.h"
#include "Particle.h"
#include "Theme.h"

#include <vector>
#include <random>

// ゲームのフェーズ管理用列挙型
// 演出も含める
enum class GAME_PHASE
{
    START = 0,			// ゲーム開始演出 初期化時に使用	
    FINISH,				// ゲーム終了演出 ResulyScene 遷移時に使用
    DO,					// ゲーム中 GameSceneExe 遷移時に使用	
	DO_UP_SPEED,		// スピードアップ演出
    DO_UP_DIFFICULTY,	// レベルアップ演出
	NUM
};

class GameSceneWait : public Scene
{
private:

    uint32_t							 m_LifeCount = 4;		// 自分のライフ数
    std::vector<std::shared_ptr<Square>> m_LifeGame;			// ライフのオブジェクト格納用
    std::shared_ptr<ParticleEmitter>	 m_LifeParticleEmitter; // ライフ減少時のパーティクルエミッター

    float m_Tick = 0.0f;
    
	std::mt19937_64 m_RandomEngine{ std::random_device{}() };

	//================================
    // 	  シーン内で使う時間関連変数
	//	  (スピード倍率によって変化)
	//================================
    TimerData m_DecrementLife = { 0.0f,0.5f };	// ライフが減るまでのタイマー管理用構造体

    //================================
    //		 ステージ遷移用フラグ
    //================================ 
	// 初期化済みかどうかのフラグ
    // また、乱数選択のリセット用にstaticで持つ
    static bool s_HasFirstGameSceneWaitInitialized;	
	static GAME_PHASE s_CurrentGamePhase;	// 現在のゲームフェーズを管理する変数

	BeatTimer m_BeatTimer;

	int m_Difficulty = 0;

	bool m_ShouldTransitionToStage = false;	// 次のステージを設定できたか判断するフラグ
    bool m_IsFirstInitialized	   = false;	// シーンが最初に初期化されたかどうかのフラグ
    bool m_wasDecrementLife		   = false;	// ライフが減ったかどうかのフラグ

    bool m_WasPlayBGM = false;				// BGMを再生したかどうかのフラグ
	int  m_PreviousBeatIndex = 0;			// 
    bool m_IsLifeTiltPositive = true;		// ライフの傾きが正かどうかのフラグ

private:
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


    //--------------------------------
    //	  ステージ乱数選択リセット要求関数
    //--------------------------------

	void RequestFullStageRandom()
	{
		s_HasFirstGameSceneWaitInitialized = false;
	}

	// 選択されたランダムなシーンへ遷移
	void StartNextStageTransition();

	void DecrementLife();
};

