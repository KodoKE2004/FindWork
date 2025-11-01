#pragma once
#include "../ObjectFile/Object.h"
#include "../Audio.h"

#include <unordered_map>
#include <vector>
class Scene
{
private:
protected:
	std::vector<Object*> m_MySceneObjects;
	std::unordered_map<std::string, AudioConfig> m_AudioList;

	int score = 0;
    int stageNo = 0;
	int stageCountMax = 5;
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


