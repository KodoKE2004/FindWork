#pragma once
#include "Scene.h"
class SelectScene : public Scene
{
public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	SelectScene() = default;
	~SelectScene() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize() ;
	// シーンの更新
	void Update()     override;

	// シーンの終了処理
	void Finalize()   ;

private:
	
};

