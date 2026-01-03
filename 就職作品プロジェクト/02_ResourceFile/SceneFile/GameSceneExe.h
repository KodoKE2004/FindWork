#pragma once
#include "Bomber.h"
#include "Player.h"
#include "RhythmBeat.h"
#include "Scene.h"
#include "Square.h"

namespace
{
	constexpr int BASE_BEATS   = 12;
}

enum class GAME_MODE
{
	NORMAL,
	ENDLESS,
	PRACTICE,
	NUM
};

class GameSceneExe : public Scene
{

// inゲーム基底クラス
protected:
	// 	共通オブジェクト
    std::shared_ptr<Skydome> m_Skydome;
	std::shared_ptr<Bomber>  m_Bomber ;		// スピードゲージ背景
    std::shared_ptr<Player>  m_Player ;		// プレイヤーオブジェクト


    // カウントダウン関連
	BeatTimer m_BeatTimer;
    
    int m_BaseBeats;

	float m_Elapsed		  = 0.0f;		// 経過時間
	float m_SpecialFrom   = 0.0f;     
	float m_SpecialTo	  = 0.0f;       
	int   m_SpecialRest   = -1;       
	bool  m_SpecialActive = false;  

	float m_FillRatio	  = 0.0f;
	float m_SegmentFrom	  = 0.0f;
	float m_SegmentTo	  = 0.0f;

	// シーン遷移フラグ
    bool  m_isChange	  = false;				// シーン変更フラグ

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange = false;					// 速攻シーン変更フラグ

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

    void SetBaseBeatCount(const int count) {
		m_BaseBeats = count; 
	}
	void StageFail() {
        m_RelationData.isClear = false;
	}
    void StageClear() { 
        m_RelationData.isClear = true;
	}
    void StageChangeFast();
	

	bool IsChange()		const { return m_isChange; }
    bool IsFastChange() const { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}    

};

