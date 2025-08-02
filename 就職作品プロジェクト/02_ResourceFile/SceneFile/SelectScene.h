#pragma once
#include "Scene.h"
class SelectScene : public Scene
{
public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	SelectScene()   { Initialize();};
	~SelectScene()  { Finalize();  };

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize() override;
	// シーンの更新
	void Update()     override;

	// シーンの終了処理
	void Finalize()   ;

private:
	
};

