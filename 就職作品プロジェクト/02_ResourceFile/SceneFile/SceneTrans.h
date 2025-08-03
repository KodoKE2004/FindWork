#pragma once
#include <memory>
#include <string>
#include "Scene.h"

enum SWITCH
{
	OFF,
	START,
	DOING,
	FINISH,
};

enum TRANS_MODE
{
	FADE,
};

/// <summary>
/// 遷移の管理をするクラス
/// </summary>
class SceneTrans
{
public:
	// コンストラクタとデストラクタ
	SceneTrans()  = default;
	~SceneTrans() = default;

	// 画面遷移の初期化
	static void Initialize(TRANS_MODE mode);
	// 画面遷移の描画
	static void Draw();

	static void	  SetTransition(SWITCH setSwitch);
	static SWITCH IsFinished();
	static void   StartTransition(TRANS_MODE mode,float duration);

private:
	static std::unique_ptr<Scene> m_NextScene;	// 次のシーン
	static float m_Timer;				// タイマー
	static float m_Duration;			// 遷移時間
	static SWITCH		m_Behavior;		// 遷移が完了したかどうか
	static TRANS_MODE	m_TransMode;	// 遷移の仕方
};

