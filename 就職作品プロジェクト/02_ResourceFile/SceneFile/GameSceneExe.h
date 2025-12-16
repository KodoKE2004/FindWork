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

    int     m_Counter = 0;				// カウントダウン用カウンター

	// 難易度・スピード関連
    float m_GameSpeedMass = 1.0f;				// ゲームスピード倍率・移動速度や制限時間まで変更
    int   m_Difficulty    = 0;					// 難易度 範囲 0 ～ 3 

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

