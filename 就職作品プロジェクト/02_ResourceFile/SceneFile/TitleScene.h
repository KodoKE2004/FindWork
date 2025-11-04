#pragma once

#include "Scene.h"
#include "../ObjectFile/Object.h"

class TitleScene : public Scene
{
private:

public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	TitleScene()  = default;
	~TitleScene() = default;

	//================================
	// 			ループ内の処理
	//================================
	
	// シーンの初期化
	void Initialize()		override;
	// シーンの更新
	void Update(float tick) override;

	// シーンの終了処理
	void Finalize()			override;

};

