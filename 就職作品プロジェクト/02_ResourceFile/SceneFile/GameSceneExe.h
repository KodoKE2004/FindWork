#pragma once
#include "Scene.h"
#include "Square.h"
#include "Bomber.h"
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
	// 	共通オブジェクト
    Skydome* m_Skydome    = nullptr;
	Bomber*  m_Bomber = nullptr;		// スピードゲージ背景

    // カウントダウン関連
	BeatTimer m_BeatTimer;
    float m_Elapsed = 0.0f;		// 経過時間
	// シーン遷移フラグ
    bool  m_isChange	  = false;				// シーン変更フラグ

	// 先行クリア時の早回し用フラグ
    bool  m_isFastChange = false;					// 速攻シーン変更フラグ

    // UVを削る数値
	float m_FillRatio = 0.0f;

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

