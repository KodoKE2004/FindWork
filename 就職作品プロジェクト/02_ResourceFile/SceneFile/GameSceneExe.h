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
    
	int m_PreciousMeasure = 0;
    int m_CurrentMeasure  = 0;

	float m_BomberElapsed = 0.0f;		// 経過時間

	int	 m_QuarterAdvance = 0;

	int   m_SpecialRest   = -1;       

	float m_FillRatio	  = 0.0f;		// 
	float m_SegmentFrom	  = 0.0f;		//
	float m_SegmentTo	  = 0.0f;		//
    float m_GameBomU	  = 0.0f;		// ゲーム内の更新時間計測

	// シーン遷移フラグ
    bool  m_isChange	  = false;		// シーン変更フラグ
	
    pShared<Audio> m_ReactionActive;		// 反応音

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange = false;					// 速攻シーン変更フラグ
	FastChangeState m_FastChangeState = FastChangeState::Filling;
	float m_FastChangeFill = 0.0f;
	float m_FastChangeStartFill = 0.0f;
	float m_FastChangeElapsed = 0.0f;


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
	// 先行クリアの処理
    void FastChange() {
		if (m_isFastChange) {
			return;
		}
		m_isFastChange = true;

		m_FastChangeState = FastChangeState::Filling;
		m_FastChangeElapsed = 0.0f;
		
		if (m_Bomber)
		{
			m_FastChangeFill = m_Bomber->GetFillRatio();
		}
		else
		{
			m_FastChangeFill = 0.0f;
		}
		m_FastChangeStartFill = m_FastChangeFill;
	}
	
	void ChangeScene();

	bool IsChange()		const { return m_isChange; }
    bool IsFastChange() const { return m_isFastChange; }
	

	// 小節が切り替わった瞬間を検知する関数
	bool IsChangeMeasure();

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::NONE;
	}    
protected:
	void Explode();
    void RegesterReactionSE(std::string seName);
};

