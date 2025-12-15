#pragma once

#include "Scene.h"
#include "Object.h"
#include "Sphere.h"
#include "Skydome.h"
#include "Square.h"

class TitleScene : public Scene
{
private:
	Square*	m_TitleLogo  = nullptr;
	Square* m_PressEnter = nullptr;
	static constexpr float AlphaChangeTimer = 0.5f;

	// UI管理の変数
	// PressEnter用
	float m_DurationPressEnter = 0.0f;

    static constexpr float TitleLogoBaseY = 50.0f;	// 基準Y座標
    static constexpr float TitleLogoAmp   = 30.0f;	// 振幅
    static constexpr float TitleLogoCycle = 7.0f;	// 周期（秒）
    float m_MoveTitleLogo = 0.0f;					// 移動用タイマー
	
public:
	//================================
	// コンストラクタとデストラクタ
	//================================
	TitleScene()  = default;
	~TitleScene() = default;


	//================================
	// 			ループ内の処理
	//================================
	
	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()			override;	// シーンの終了処理

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::TITLE;
	}


};

