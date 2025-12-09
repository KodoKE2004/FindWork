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
	Bomber* m_TimeGauge = nullptr;		// スピードゲージ背景
    float	m_TimeGaugeRatio = 1.0f;	// スピードゲージの割合
    float	m_TimeGaugeStep  = 0.0f;	// スピードゲージの減少ステップ量

    BomTimeLimit* m_Number = nullptr;	// カウントダウン用数字
    int     m_Counter = 0;				// カウントダウン用カウンター

    static constexpr float m_GameSceneTimeBase = 5.0f;	// ゲーム時間の基準値
    TimerData m_TimeChangeScene = {0.0f, 5.0f};			// 実際に扱うシーン変更までの時間

    float m_GameSpeedMass = 1.0f;						// ゲームスピード倍率・移動速度や制限時間まで変更
    int   m_Difficulty    = 0;							// 難易度 範囲 0 ～ 3 
    bool  m_isChange	  = false;						// シーン変更フラグ

	// 先行でステージをクリア場合に適応するフラグ
    float m_ChangeFastTime  = 0.0f;						// 速攻でシーン変更する時間
    float m_ChangeFastTimer = 0.0f;						// 速攻でシーン変更するタイマー
    bool  m_isFastChange = false;						// 速攻シーン変更フラグ


	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Skydome* m_Skydome    = nullptr;
public:
	//================================
	// コンストラクタとデストラクタ
	//================================
	GameSceneExe()  = default;
	~GameSceneExe() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	virtual void Initialize()		;
	// シーンの更新
	virtual void Update(float tick) ;
	// シーンの終了処理
	virtual void Finalize()			;


	bool IsChange() const { return m_isChange; }

    void TimeCountFast(const float tick) { m_ChangeFastTimer += tick; }
    bool IsFastTimeUp() const			 { return m_ChangeFastTimer >= m_ChangeFastTime; }
    bool IsFastChange() const			 { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}

	void FastChangeScene(float time) ;
    

};

