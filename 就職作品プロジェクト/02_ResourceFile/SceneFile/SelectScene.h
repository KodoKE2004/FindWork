#pragma once
#include "Scene.h"
#include "Sphere.h"
#include "Square.h"

#include "Game.h"
#include <memory>
#include <vector>

class SelectScene : public Scene
{
private:

	std::shared_ptr<Square> m_ButtonGamePlay ;
	std::shared_ptr<Square> m_ButtonPractice ;
    std::shared_ptr<Square> m_Corsor		 ;

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

