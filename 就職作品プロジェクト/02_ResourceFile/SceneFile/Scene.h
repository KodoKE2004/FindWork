#pragma once
#include "Object.h"
#include "TransitionBase.h"
#include "Skydome.h"
#include "../Audio.h"
#include "RhythmBeat.h"

#include <unordered_map>
#include <vector>

enum class SCENE_NO
{
	NONE = -1,
	TITLE,
	SELECT,
	RESULT,
	GAME_WAIT,
	GAME_EXE,
    GAME_SLICE,
	GAME_JUMP,
	GAME_CRUSH,
	TRANSITION,
	NUM
};

// Sceneクラス間の受け渡しデータ
// isClear       : ミニゲームをクリアしたかのフラグ
// requestRetry  : ゲームをもう一度するかのフラグ
// stageCount    : 何ステージクリアしているか
// previousScene : 現在のシーン
// oldScene      : 一つ前のシーン
// nextScen      : 次のシーン
class SceneRelationData
{
public:
	bool isClear      = true;
	uint32_t gameLife = 4;
	int  stageCount   = 0;
	SCENE_NO previousScene = SCENE_NO::NONE;
    SCENE_NO oldScene	   = SCENE_NO::NONE;
	SCENE_NO nextScene     = SCENE_NO::NONE;
};

class Scene
{
protected:
	// PressEnterの点滅タイマー間隔
    static constexpr float DEFAULT_VALUME = 0.2f; 
protected:
	
	std::vector<Object*> m_MySceneObjects;
	std::unordered_map<std::string, AudioConfig> m_AudioList;
	
	Skydome* m_Skydome = nullptr;

	const int stageCountMax = 5;

	TransitionBase* m_TransitionTexture = nullptr;

	// ExeSceneで使う変数
	float m_TimerGameExe = 0.0f;			// 経過時間
    static std::vector<float*>	m_TimerList;	// タイマー格納用

public:
	static SceneRelationData m_RelationData;
	// コンストラクタとデストラクタ
	Scene()  = default;
	virtual ~Scene() = default;

	//---------------------------------
	// 			ループ内の処理
	//		 純粋仮想関数として定義
	//---------------------------------

	virtual void Initialize()		= 0;	// 初期化処理
	virtual void Update(float tick) = 0;	// 更新処理
	virtual void Finalize()			= 0;	// 解放処理

	// そのシーンのオブジェクトを定義
	std::vector<Object*> GetSceneObjects() {
		return m_MySceneObjects;
	}

	virtual SCENE_NO GetSceneNo() const = 0;

    //--------------------------------
	//	  シーン間の受け渡しの値をする関数
    //--------------------------------
	void SetRelationData(const SceneRelationData relationData) {
		m_RelationData = relationData; 
	}
	const SceneRelationData GetRelationData() {
		return m_RelationData;
	}

    //--------------------------------
    // タイマー関連の関数群
    //--------------------------------
	static void SetTimer(float* timer) 
	{
		*timer = 0.0f;
		m_TimerList.emplace_back(timer);
	}

	// それぞれが作成したTimer変数をカウント
	void CountTimer(const float tick)
	{	
		for (auto timer : m_TimerList)
		{
			*timer += tick;
		}
    }

    // 時間制限を超えたか判定する関数群
	bool IsTimeUp(const float limit) const {
		return m_TimerGameExe >= limit; 
	}
	bool IsTimeUp(const float time ,const float limit) const {
		return time >= limit; 
	}

    // 効果音再生関数
    // seName : AudioConfigに登録した名前
    // Game.hのAudioManagerを通じて再生する
	void PlaySE(std::string seName);
};


