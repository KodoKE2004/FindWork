#pragma once
#include "Scene.h"
#include "Square.h"

enum class DIFFICULT
{
	NORMAL,
	iiiENDLESS,
	NUM
};


class GameSceneExe : public Scene
{
// inゲーム基底クラス
protected:

    float m_Tick = 0.0f;					// 経過時間
    float m_ChangeSceneTime = 5.0f;			// シーン変更までの時間
	float m_GameSpeedMass = 1.0f;
	bool  m_isChange = false;

	//================================
	// 	このシーンで使うオブジェクト
    //================================
    Square* m_Background = nullptr;

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

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_EXE;
	}

};

