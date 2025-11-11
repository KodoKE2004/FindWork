#pragma once
#include "Scene.h"
#include "Square.h"
#include "Sphere.h"

#include "Game.h"
#include <vector>
#include <memory>
class SelectScene : public Scene
{
private:

	Square* m_ButtonGamePlay = nullptr;
	Square* m_ButtonPractice = nullptr;
    Square* m_Corsor		 = nullptr;

public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	SelectScene()  = default;

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::SELECT;
	}

	~SelectScene() = default;

	//================================
	// 			ループ内の処理
	//================================

	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()			override;	// シーンの終了処理


};

