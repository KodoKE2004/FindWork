#pragma once
#include "Scene.h"
#include "Square.h"
#include "Particle.h"
#include "Theme.h"

#include <vector>
#include <random>

// ゲーム全体進行の待機/案内フェーズ。
// Exeシーン間の橋渡しを行い、次ステージ準備・UI演出・難易度/BPM調整を担当する。
enum class GAME_PHASE
{
	DO,                 // 通常の次ステージ待機
	START,              // ゲーム開始演出
	FINISH,             // ゲーム終了演出（Result遷移前）
	DO_UP_SPEED,        // スピードアップ告知
	DO_UP_DIFFICULTY,   // 難易度アップ告知
	NUM
};

enum class UI_PHASE
{
	NONE = -1,
	SLIDE_IN,
	WAIT,
	SLIDE_OUT,
	NUM
};

class GameSceneWait : public Scene
{
private:
	uint32_t			 m_LifeCount = 4;       // 現在保持しているライフ数
	vector<pShared<Square>>  m_LifeGame;           // ライフUIの実体リスト
	pShared<Square>		 m_GameUI;               // 案内UI（スライド表示）
	vector<pShared<Square>>  m_StageNumber;         // ステージ番号表示用

	float m_Tick = 0.0f;
	int	  m_QuarterAdvance = 0;                      // 更新前の四分音符基準位置

	std::mt19937_64 m_RandomEngine{ std::random_device{}() };

	// ライフ減少を即時にしないための猶予タイマー。
	TimerData m_DecrementLife = { 0.0f,0.5f };

	// 待機シーンは複数回入るため、フェーズ状態を static で共有している。
	static GAME_PHASE m_CurrentGamePhase;

	struct UIPhaseInfo {
		UI_PHASE phase = UI_PHASE::NONE;
		float	 moveValueX = 0.0f; // 目標までの移動量
		float	 movementTime = 0.0f; // 移動にかける秒数
		float	 movementElapsed = 0.0f; // 移動経過秒
		bool	 isBoot = false;           // UI演出を有効化するトリガ
	};

	UIPhaseInfo m_UIGame;
	UIPhaseInfo m_UIStage;

	float m_ScalingLifeElapsed = 0.0f;          // ライフ拡縮演出の経過秒
	float m_ScalingLifeDuration = 0.0f;         // ライフ拡縮演出の1フェーズ時間
	NVector3 m_LifeBaseScale;                   // ライフ表示の基準スケール

	bool m_ShouldTransitionToStage = false;     // 次ステージへ遷移可能になったか
	bool m_IsFirstInitialized = false;          // 最初の Wait 初期化かどうか
	bool m_wasDecrementLife = false;			// 今回の待機でライフ減算済みか
	bool m_isLifeScaleUp = false;
	bool m_isLifeScaleDown = false;
	bool m_isPendingBpmChange = false;          // UI演出タイミングでBPM更新待ちか

private:
	// 次に遷移する Exe シーンを選択し、RelationData へ確定する。
	void PrepareNextStage();

	// 案内UIを生成して表示シーケンスを開始する。
	void RegisterGameUI(pShared<Texture> texture, float u, float v);
	// 案内UIのスライド演出を拍進行に合わせて更新する。
	void GameUIMovement(int elapsedBeat);
	// 次ステージへの遷移実行。
	void StartNextStageTransition();
	// ライフを1つ減らす。
	void LifeDecrement();
	// ライフの脈動演出。
	void LifeScaling();

public:
	GameSceneWait() = delete;
	GameSceneWait(Camera& cam);
	~GameSceneWait() = default;

	void Initialize() override;
	void Update(float tick) override;
	void Draw() override;
	void Finalize() override;

	bool IsFirstInitialized() const {
		return m_IsFirstInitialized;
	}

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_WAIT;
	}
};

