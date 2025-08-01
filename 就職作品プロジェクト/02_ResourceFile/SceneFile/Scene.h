#pragma once

class Scene
{
private:

public:
	// コンストラクタとデストラクタ
	Scene()  = default;
	virtual ~Scene() = default;

	//================================
	// 			ループ内の処理
	//		 純粋仮想関数として定義
	//================================
	
	// シーンの更新
	virtual void Update() = 0;

};


