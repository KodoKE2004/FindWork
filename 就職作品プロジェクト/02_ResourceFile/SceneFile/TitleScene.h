#pragma once

#include "Scene.h"
#include "Object.h"
#include "Sphere.h"
#include "Skydome.h"
#include "Square.h"

class TitleScene : public Scene
{
private:
	Square*	 m_TitleLogo = nullptr;
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

