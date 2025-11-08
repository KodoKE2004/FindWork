#pragma once

#include "Scene.h"
#include "Object.h"
#include "Sphere.h"
#include "Skydome.h"
#include "Square.h"

class TitleScene : public Scene
{
private:
	Skydome*   m_Skydome   = nullptr;
	Square* m_TitleLogo = nullptr;
public:
	//================================
	// コンストラクタとデストラクタ
	//================================
	TitleScene()  = default;

	SceneNo GetSceneNo() const override {
		return SceneNo::TITLE;
	}

	~TitleScene() = default;

	//================================
	// 			ループ内の処理
	//================================
	
	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()			override;	// シーンの終了処理


};

