#pragma once
#include "../ObjectFile/Object.h"
#include <vector>
class Scene
{
private:
protected:
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト
public:
	// コンストラクタとデストラクタ
	Scene()  = default;
	virtual ~Scene() = default;

	//================================
	// 			ループ内の処理
	//		 純粋仮想関数として定義
	//================================
	
	// シーンの更新
	virtual void Initialize() = 0;
	virtual void Update(float tick) = 0;
	virtual void Finalize() = 0;

	// そのシーンのオブジェクトを定義
	std::vector<Object*> GetSceneObjects() {
		return m_MySceneObjects;
	}
};


