#pragma once
#include "Scene.h"
#include "Square.h"
#include "Bomber.h"
#include "Timer.h"
#include "RhythmBeat.h"

namespace
{
	constexpr int BASE_BEATS   = 11;
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
	std::shared_ptr<Bomber> m_Bomber ;		// スピードゲージ背景
	std::shared_ptr<Timer>  m_TimerUI;		// スピードゲージ背景

    // カウントダウン関連
	BeatTimer m_BeatTimer;
    
    int m_BaseBeats;

	float m_BomberElapsed = 0.0f;		// 経過時間

	float m_SpecialFrom   = 0.0f;     
	float m_SpecialTo	  = 0.0f;       
	int   m_SpecialRest   = -1;       
	bool  m_SpecialActive = false;  

	float m_FillRatio	  = 0.0f;		// 
	float m_SegmentFrom	  = 0.0f;
	float m_SegmentTo	  = 0.0f;

	// シーン遷移フラグ
    bool  m_isChange	  = false;				// シーン変更フラグ

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange = false;					// 速攻シーン変更フラグ
	FastChangeState m_FastChangeState = FastChangeState::Filling;
	float m_FastChangeFill = 0.0f;
	int   m_PreviousBarIndex = 0;
public:
	//================================
	// コンストラクタとデストラクタ
	//================================
	GameSceneExe()  = default;
	~GameSceneExe() = default;

	//================================
	// 			ループ内の処理
	//================================

	virtual void Initialize()		;
	virtual void Update(float tick) ;
	virtual void Finalize()			;

    /// <summary>
	/// シーンの存在する拍数を設定
    /// </summary>
    /// <param name="count"></param>
    void SetBaseBeatCount(const int count) {
		m_BaseBeats = count; 
	}
	void StageFail() {
        m_RelationData.isClear = false;
	}
    void SetStageClear() { 
        m_RelationData.isClear = true;
	}
	// 先行クリアの処理
    void SetFastChange() {
		if (m_isFastChange) {
			return;
		}
		m_isFastChange = true;

		m_FastChangeState = FastChangeState::Filling;
		if (m_Bomber)
		{
			m_FastChangeFill = m_Bomber->GetFillRatio();
		}
		else
		{
			m_FastChangeFill = 0.0f;
		}
	}
	


	bool IsChange()		const { return m_isChange; }
    bool IsFastChange() const { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::NONE;
	}    
protected:
	void Explode();
};

