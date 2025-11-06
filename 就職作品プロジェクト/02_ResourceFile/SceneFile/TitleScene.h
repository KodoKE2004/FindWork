#pragma once

#include "Scene.h"
#include "Object.h"
#include "Sphere.h"

class TitleScene : public Scene
{
private:
	std::shared_ptr<Sphere> m_SkyDome;
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

