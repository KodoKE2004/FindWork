#pragma once
#include "Scene.h"
#include "../ObjectFile/Object.h"
#include "../TestFile/TestPlane.h"

class TitleScene : public Scene
{
private:
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト
public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	TitleScene()  { Initialize();}
	~TitleScene() { Finalize();  }

	//================================
	// 			ループ内の処理
	//================================
	
	// シーンの初期化
	void Initialize() ;
	// シーンの更新
	void Update()     override;

	// シーンの終了処理
	void Finalize();

};

