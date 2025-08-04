#pragma once
#include <memory>
#include <string>
#include "Scene.h"
#include "../Renderer.h"
#include "../Application.h"
#include "../Texture2D.h"
#include "../Game.h"
#include <vector>

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
	void Initialize(TRANS_MODE mode);
	// 画面遷移の更新
	void Update();
	// 画面遷移の描画
	void Draw();
	void   SetAlpha(float alpha);
	void   SetChange(bool isChange);
	float  GetAlpha(){ return m_Alpha; }
	void   SetTransition(SWITCH setSwitch);
	SWITCH IsTransition();

	void StartTransition(TRANS_MODE mode, Scene* nextScene, float duration);


	void FadeInit();
	void FadeIN();
	void FadeOUT();





private:
	Scene*		m_NextScene;		// 次のシーン
	float		m_Timer;			// タイマー
	float		m_Duration;			// 遷移時間
	bool		m_isChange;			// 切り替えOK？
	SWITCH		m_isTransition;		// 遷移が完了したかどうか
	TRANS_MODE	m_TransMode;		// 遷移の仕方
	Texture2D*	m_Texture;

	// 演出の値管理
	float m_Alpha;
	float m_Delta;


};

