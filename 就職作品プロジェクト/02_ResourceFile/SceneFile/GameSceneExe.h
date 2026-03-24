#pragma once
#include "Scene.h"
#include "Square.h"
#include "Bomber.h"
#include "Timer.h"

namespace
{
    // 基本の小節
	constexpr int BASE_BEATS  = 24;
    constexpr int ONE_MEASURE = 8;
}

enum class GAME_MODE
{
	NORMAL,
	ENDLESS,
	PRACTICE,
	NUM
};

// 先行クリア用列挙
enum class FastChangeState
{
	Filling,
	ReadyToExplode,
	Exploded
};

class GameSceneExe : public Scene
{

// inゲーム基底クラス
protected:
	// 	共通オブジェクト
	pShared<Bomber> m_Bomber;		// スピードゲージ背景
	pShared<Timer>  m_TimerUI;		// スピードゲージ背景
    
	int m_PreciousMeasure = 0;		// 前フレーム時点の小節
    int m_CurrentMeasure  = 0;		// 今フレーム時点の小節

	float m_BomberElapsed = 0.0f;		 // 1区間内での経過時間
	int	 m_QuarterAdvance = 0;			 // 更新前の「四分音符基準の進行位置」

	float m_FillRatio	  = 0.0f;		 // 現在のゲージ充填率
	float m_SegmentFrom	  = 0.0f;		 // 今回補完の開始値
	float m_SegmentTo	  = 0.0f;		 // 今回補完の終了値
    bool  m_isChange	  = false;		 // シーン遷移フラグ

    pShared<Audio> m_ReactionActive;	 // 反応音

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange		= false; // 速攻シーン変更フラグ
	FastChangeState m_FastChangeState = FastChangeState::Filling;
	float m_FastChangeFill		= 0.0f;	 // 早回し開始時点のゲージ量
	float m_FastChangeStartFill = 0.0f;	 // 早回し補完の始点
	float m_FastChangeElapsed	= 0.0f;	 // 早回し演出の経過時間

public:

	//================================
	// コンストラクタとデストラクタ
	//================================
	GameSceneExe()  = delete;
	GameSceneExe(Camera& cam);
	~GameSceneExe() = default;

	//================================
	// 			ループ内の処理
	//================================

	virtual void Initialize()		;
	virtual void Update(float tick) ;
	virtual void Draw()	= 0;
	virtual void Finalize()			;

	void StageFail() {
        m_RelationData.isClear = false;
	}
    void StageClear() { 
        m_RelationData.isClear = true;
	}

    void FastChange();
	void ChangeScene();

	bool IsChange()		const { return m_isChange; }
    bool IsFastChange() const { return m_isFastChange; }
	
	// 前フレームと今フレームの小節番号を比較し、
	// 小節が切り替わった瞬間だけtrueを返す
	bool IsChangeMeasure();

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::NONE;
	}

protected:
	// 爆弾の爆発処理
	void Explode();
	// SEの登録
    void RegisterReactionSE(std::string seName);

};

