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
    DO,					// ゲーム中 GameSceneExe 遷移時に使用	
    START ,				// ゲーム開始演出 初期化時に使用	
    FINISH,				// ゲーム終了演出 ResulyScene 遷移時に使用
	DO_UP_SPEED,		// スピードアップ演出
    DO_UP_DIFFICULTY,	// レベルアップ演出
	NUM
};

enum class UI_PHASE
{
    NONE = -1,
	SLIDE_IN,	// ゲームUIが入ってくるフェーズ
	WAIT,		// ゲームUIが中央で待機しているフェーズ
	SLIDE_OUT,	// ゲームUIが出ていくフェーズ
    NUM
};

class GameSceneWait : public Scene
{
private:

    uint32_t				 m_LifeCount = 4;	// 自分のライフ数
    vector<pShared<Square>>  m_LifeGame;		// ライフのオブジェクト格納用
	pShared<Square>			 m_GameUI;			// ライフ減少時のパーティクルエミッター

    float m_Tick = 0.0f;
	int	  m_QuarterAdvance = 0;

	std::mt19937_64 m_RandomEngine{ std::random_device{}() };

    // シーン内で使う時間関連変数
	//(スピード倍率によって変化)
    TimerData m_DecrementLife = { 0.0f,0.5f };	// ライフが減るまでのタイマー管理用構造体

    // ステージ遷移用フラグ
	// 初期化済みかどうかのフラグ
    // また、乱数選択のリセット用にstaticで持つ
	static GAME_PHASE m_CurrentGamePhase;			// 現在のゲームフェーズを管理する変数
    UI_PHASE m_CurrentUIPhase  = UI_PHASE::NONE;	// 現在のゲームUIフェーズを管理する変数
    float m_GameUIMoveValueX	  = 0.0f;			// ゲームUIの移動量
    float m_GameUIMovementTime	  = 0.0f;			// ゲームUIの移動タイマー
    float m_GameUIMovementElapsed = 0.0f;			// 現在のビートの経過時間
	bool  m_isBootGameUI		  = false;

    float m_ScalingLifeElapsed  = 0.0f;				// ライフのスケーリング演出のタイマー
    float m_ScalingLifeDuration = 0.0f;				// ライフのスケーリング演出の時間
    NVector3 m_LifeBaseScale;				// ライフのスケーリング演出の開始時のスケール

	bool m_ShouldTransitionToStage = false;	// 次のステージを設定できたか判断するフラグ
    bool m_IsFirstInitialized	   = false;	// シーンが最初に初期化されたかどうかのフラグ
    bool m_wasDecrementLife		   = false;	// ライフが減ったかどうかのフラグ
    bool m_isLifeScaleUp		   = false;	// ライフ減少の演出でハートが大きくなっているかどうかのフラグ
    bool m_isLifeScaleDown		   = false;	// ライフ減少の演出でハートが小さくなっているかどうかのフラグ
	bool m_isPendingBpmChange	   = false; // BPMの変更を行うフェーズか
private:
	// Exeシーンの乱数選択を行う。
	// 二回連続で同じステージが来るようにならないようにする 
	// 要素の削除の仕方は考える。
	void PrepareNextStage();

    // ゲームUIの登録
	void RegisterGameUI(pShared<Texture> texture, float u, float v);
    // ゲームUIの移動処理
    void GameUIMovement(int elapsedBeat);
	// 選択されたランダムなシーンへ遷移
	void StartNextStageTransition();
    // ライフを減らす処理
	void DecrementLife();
	// ライフのスケーリング演出
	void ScalingLife();
public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	GameSceneWait()  = delete;
	GameSceneWait(Camera& cam);
	~GameSceneWait() = default;

	//================================
	// 			ループ内の処理
	//================================

	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Draw() 			override;
	void Finalize()         override;	// シーンの終了処理


	bool IsFirstInitialized() const {
		return m_IsFirstInitialized;
    }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_WAIT;
	}


};

