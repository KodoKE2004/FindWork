#pragma once
#include "Scene.h"
#include "Square.h"

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

    float m_Tick = 0.0f;					// 経過時間
    float m_ChangeSceneTime = 5.0f;			// シーン変更までの時間
    float m_GameSpeedMass = 1.0f;			// ゲームスピード倍率
    int   m_DifficultLevel = 1;				// 難易度レベル
    bool  m_isChange = false;				// シーン変更フラグ

    float m_ChangeFastTime = 0.0f;			// 速攻でシーン変更する時間
    float m_ChangeFastTimer = 0.0f;			// 速攻でシーン変更するタイマー
    bool  m_isFastChange = false;			// 速攻シーン変更フラグ


	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Square* m_Background = nullptr;
    Skydome* m_Skydome = nullptr;
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

    void TickCount(const float tick) { m_Tick += tick; }
    bool IsTimeUp() const			 { return m_Tick >= m_ChangeSceneTime; }
	bool IsChange() const			 { return m_isChange; }

    void TimeCountFast(const float tick) { m_ChangeFastTimer += tick; }
    bool IsFastTimeUp() const			 { return m_ChangeFastTimer >= m_ChangeFastTime; }
    bool IsFastChange() const			 { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}

	void FastChangeScene(float time) ;
    

};

