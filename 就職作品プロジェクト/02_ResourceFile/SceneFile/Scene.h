#pragma once
#include "ObjectFile/Object.h"
#include "../Audio.h"

#include <unordered_map>
#include <vector>

enum class SceneNo
{
	NONE = -1,
	TITLE,
	SELECT,
	GAME_WAIT,
	GAME_EXE,
	TRANSITION,
	NUM
};

struct SceneRelationData
{
	bool isClear = false;
	int  score = 0;
	int  stageNo = 0;
	bool requestRetry = false;
	SceneNo previousScene = SceneNo::NONE;
	SceneNo nextScene     = SceneNo::NONE;
};

class Scene
{
protected:
	std::vector<std::shared_ptr<Object>> m_MySceneObjects;
	std::unordered_map<std::string, AudioConfig> m_AudioList;

	SceneRelationData m_RelationData;
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
	std::vector<std::shared_ptr<Object>> GetSceneObjects() {
		return m_MySceneObjects;
	}

	virtual SceneNo GetSceneNo() const = 0;

	// シーン間の受け渡しの値をする関数（それぞれのゲッター・セッター）
	void SetRelationData(const SceneRelationData relationData) {
		m_RelationData = relationData; 
	}
	SceneRelationData& AccessRelationData()			 { return m_RelationData; }
	const SceneRelationData& GetRelationData() const { return m_RelationData; }

};


