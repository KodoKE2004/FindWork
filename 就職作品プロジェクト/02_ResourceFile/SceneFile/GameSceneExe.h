#pragma once
#include "Scene.h"
#include "Square.h"
#include "Bomber.h"
#include "Timer.h"

namespace
{
	// GameSceneExe 系で共通利用する拍基準。
	constexpr int BASE_BEATS = 24;
	constexpr int ONE_MEASURE = 8;
}

enum class GAME_MODE
{
	NORMAL,
	ENDLESS,
	PRACTICE,
	NUM
};

// 先行クリア時の「早回し→爆発」演出ステート。
enum class FastChangeState
{
	Filling,
	ReadyToExplode,
	Exploded
};

class GameSceneExe : public Scene
{
protected:
	// 共通UIオブジェクト
	pShared<Bomber> m_Bomber;      // ボンバーUI（残量ゲージ＋カウントダウン表示）
	pShared<Timer>  m_TimerUI;     // タイマーUI本体

	// 小節切り替え検知用。前フレーム値を保持して境界跨ぎを検出する。
	int m_PreciousMeasure = 0;
	int m_CurrentMeasure = 0;

	float m_BomberElapsed = 0.0f;  // 現在区間（1拍相当）での経過秒
	int	 m_QuarterAdvance = 0;     // 更新前の四分音符基準の進行位置

	float m_FillRatio = 0.0f;   // 内部進行率（0→1）
	float m_SegmentFrom = 0.0f; // 補間区間の始点
	float m_SegmentTo = 0.0f;   // 補間区間の終点
	bool  m_isChange = false;	// 次シーンへ進む確定フラグ

	pShared<Audio> m_ReactionActive; // このステージで鳴らす反応SEの再生ハンドル

	// 先行クリア時の専用フロー。通常進行を止めて短時間でゲージを落とす。
	bool  m_isFastChange = false;
	FastChangeState m_FastChangeState = FastChangeState::Filling;
	float m_FastChangeFill = 0.0f;
	float m_FastChangeStartFill = 0.0f; // 早回し開始時の残量を記録して補間始点に使う
	float m_FastChangeElapsed = 0.0f;

public:
	GameSceneExe() = delete;
	GameSceneExe(Camera& cam);
	~GameSceneExe() = default;

	// GameSceneExe 共通初期化。
	// ボンバーUI、SE、拍進行関連の初期状態を整える。
	virtual void Initialize();

	// 実行シーン共通更新。
	// リズム更新→区間進行更新→UI反映→遷移判定の順で進める。
	virtual void Update(float tick);
	virtual void Draw() = 0;
	virtual void Finalize();

	void StageFail() {
		m_RelationData.isClear = false;
	}
	void StageClear() {
		m_RelationData.isClear = true;
	}

	// 先行クリア時の早回し演出を開始する。
	void FastChange();

	// Wait シーンへ戻すための共通遷移。
	void ChangeScene();

	bool IsChange()		const { return m_isChange; }
	bool IsFastChange() const { return m_isFastChange; }

	// 前フレームと現フレームの小節番号差分から、
	// 小節境界を跨いだかを判定する。
	bool IsChangeMeasure();

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::NONE;
	}

protected:
	// 爆発演出を確定し、シーン遷移可能状態へ切り替える。
	void Explode();
	// 反応SEを名前で生成して保持する。
	void RegisterReactionSE(std::string seName);
};
