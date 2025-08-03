#pragma once
#include <memory>
#include <string>
#include "Scene.h"

enum SWITCH
{
	OFF,
	START,
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

	// 画面遷移の更新
	static void Update();
	// 画面遷移の描画
	static void DrawOverlay();

	static void StartTransition(TRANS_MODE mode);

	static void SetTransition(SWITCH setSwitch);
	static SWITCH IsFinished();

private:
	static std::unique_ptr<Scene> m_NextScene;	// 次のシーン
	static float m_Timer;				// タイマー
	static float m_Duration;			// 遷移時間
	static SWITCH		m_Behevior;		// 遷移が完了したかどうか
	static TRANS_MODE	m_TransMode;	// 遷移の仕方
	static std::string m_EffectType;	// "Fade", "Wipe" etc...
};

