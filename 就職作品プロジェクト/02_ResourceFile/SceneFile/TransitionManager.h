#pragma once
#include <memory>
#include <string>
#include "Scene.h"
/// <summary>
/// 遷移の管理をするクラス
/// </summary>
class TransitionManager 
{
public:
	// コンストラクタとデストラクタ
	TransitionManager()  = default;
	~TransitionManager() = default;

	// 画面遷移の更新
	void Update();
	// 画面遷移の描画
	void DrawOverlay();
	// 画面遷移の終了処理
	bool IsFinished();

private:
	std::unique_ptr<Scene> m_NextScene;	// 次のシーン
	float m_Timer = 0.0f;				// タイマー
	float m_Duration = 1.0f;			// 遷移時間
	bool  m_Finished = false;			// 遷移が完了したかどうか
	std::string m_EffectType = "Fade";	// "Fade", "Wipe" etc...
};

