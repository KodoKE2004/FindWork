#pragma once
#include "Scene.h"
#include "Square.h"
#include "Bomber.h"
#include "BomTimeLimit.h"
#include "RhythmBeat.h"

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
	//================================
	// 	共通オブジェクト
    //================================
    Skydome* m_Skydome    = nullptr;
	Bomber*  m_Bomber = nullptr;		// スピードゲージ背景

    //================================
	//			制限時間ゲージ群
    //================================
    float	m_TimeGaugeRatio = 1.0f;	// スピードゲージの割合
    float	m_TimeGaugeStep  = 0.0f;	// スピードゲージの減少ステップ量
	
	float m_GaugeStartRatio   = 1.0f;	// この拍の頭での比率
	float m_GaugeTargetRatio  = 1.0f;	// この拍で到達したい比率
	float m_GaugeBeatElapsed  = 0.0f;	// 拍頭からの経過時間(秒)
	float m_GaugeAnimDuration = 0.0f;	// 1拍の中で実際に動かす時間(秒)

    int     m_Counter = 0;				// カウントダウン用カウンター

	static constexpr int   GaugeTicks = 10;		// 拍で遷移の確認をとる

	// 難易度・スピード関連
    float m_GameSpeedMass = 1.0f;				// ゲームスピード倍率・移動速度や制限時間まで変更
    int   m_Difficulty    = 0;					// 難易度 範囲 0 ～ 3 

	// シーン遷移フラグ
    bool  m_isChange	  = false;				// シーン変更フラグ
	bool  m_hasRequestedSceneChange = false;	// 遷移要求

    int m_ElapsedBeats = 0;						// 経過拍数
    int m_PreviousBeatIndex = 0;				// 前回の拍数

	// 一定小節が経過したら強制的に待機に戻す
	static constexpr int ForcedReturnMeasures = 3;
	int m_ForcedReturnBeatCount = 0;			// 強制リターンまでの拍数

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange = false;					// 速攻シーン変更フラグ
    float m_OneMeasure   = 0.0f;					// １小節の時間
	bool  m_ShouldSkipNextBoundary = false;			// 

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

	void StageFail()
	{
        m_RelationData.isClear = false;
	}
    void StageClear()	  
	{ 
        m_RelationData.isClear = true;
	}
    void StageChangeFast();
	

	bool IsChange()		const { return m_isChange; }
    bool IsFastChange() const { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}    

};

