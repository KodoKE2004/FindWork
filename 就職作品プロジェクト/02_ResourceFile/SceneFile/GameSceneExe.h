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

    // ExeSceneで使う変数
    float m_Duration = 0.0f;							// 経過時間
    static constexpr float m_GameSceneTimeBase = 5.0f;	// ゲーム時間の基準値
    float m_ChangeSceneTime = 5.0f;						// 実際に扱うシーン変更までの時間
    float m_GameSpeedMass = 1.0f;						// ゲームスピード倍率・移動速度や制限時間まで変更
    int   m_Difficulty = 0;								// 難易度 範囲 0 ～ 3 
    bool  m_isChange = false;							// シーン変更フラグ

	// 先行でステージをクリア場合に適応するフラグ
    float m_ChangeFastTime = 0.0f;						// 速攻でシーン変更する時間
    float m_ChangeFastTimer = 0.0f;						// 速攻でシーン変更するタイマー
    bool  m_isFastChange = false;						// 速攻シーン変更フラグ


	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Square*  m_Background = nullptr;
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

    void TickCount(const float tick) { m_Duration += tick; }
    bool IsTimeUp() const			 { return m_Duration >= m_ChangeSceneTime; }
	bool IsChange() const			 { return m_isChange; }

    void TimeCountFast(const float tick) { m_ChangeFastTimer += tick; }
    bool IsFastTimeUp() const			 { return m_ChangeFastTimer >= m_ChangeFastTime; }
    bool IsFastChange() const			 { return m_isFastChange; }

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}

	void FastChangeScene(float time) ;
    

};

