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
	Square* m_FadeMask   = nullptr;
	static constexpr float AlphaChangeTimer = 0.5f;

	// UI管理の変数
	// PressEnter用
	float m_DurationPressEnter = 0.0f;

	// TitleLogo用
	static constexpr float TitleLogoBaseY = 50.0f;
	static constexpr float TitleLogoAmp   = 30.0f;
	static constexpr float TitleLogoCycle = 7.0f ; 
	float m_MoveTitleLogo = 0.0f;
	
	// フェードマスク用
	// ゲームを始めた瞬間のフェードをTransSceneに備えていない。
	// そのため開幕につかわれるタイトルシーンでは始める時だけ自前でフェード演出を行う。
	bool m_EntryFlg = false;
	static constexpr float m_FadeTimer = 10.0f; 
	
	float m_DurationFadeMask = 0.0f;
	

public:
	//================================
	// コンストラクタとデストラクタ
	//================================
	TitleScene()  = default;
	TitleScene(bool entryFlg);
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

