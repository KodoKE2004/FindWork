#pragma once
#include "Object.h"
#include "TransitionBase.h"
#include "Skydome.h"
#include "../Audio.h"
#include "RhythmBeat.h"
#include "Camera.h"

#include <optional>
#include <unordered_map>
#include <vector>

class Theme;

enum class SCENE_NO
{
    NONE = -1,
	GAME_DODGE = 0,
	GAME_SHOT,
	GAME_TEXT,
	// GAME_ROCKET,
	// GAME_GUNMAN,
	EXE_NUM,
	TITLE,
	RESULT,
	TRANSITION,
	GAME_WAIT,
};

// Sceneクラス間の受け渡しデータ
// isClear       : ミニゲームをクリアしたかのフラグ
// requestRetry  : ゲームをもう一度するかのフラグ
// stageCount    : 何ステージクリアしているか
// previousScene : 現在のシーン
// oldScene      : 一つ前のシーン
// nextScene     : 次のシーン
struct SceneRelationData
{
	uint32_t gameLife = 4;
	int  stageCount   = 0;
    SCENE_NO oldScene	   = SCENE_NO::NONE;
	SCENE_NO nextScene     = SCENE_NO::NONE;
	pShared<Square> transTexture = nullptr;
	bool isClear      = true;
    bool pad[3]		  = { false,false,false };

	void SetTransitionTarget(pShared<Square> transition)
	{
		if (transition == nullptr) return;
		this->transTexture = transition;
	}

	void ClearTransitionTexture()
	{
		transTexture = nullptr;
	}
};

class Scene
{
protected:

	// PressEnterの点滅タイマー間隔
    static constexpr float DEFAULT_VOLUME = 0.2f; 
protected:
	
	vector<pShared<Object>>		 m_MySceneObjects;
	std::unordered_map<std::string, AudioConfig> m_AudioList;
	pShared<Skydome> m_Skydome = nullptr;
	pShared<Theme>	 m_Theme   = nullptr;
	Camera&					 m_Camera;

	const int stageCountMax = 5;

	TransitionBase* m_TransitionTexture = nullptr;

	// ExeSceneで使う変数
	float m_TimerGameExe = 0.0f;				// 経過時間
    static vector<float*>	m_TimerList;	// タイマー格納用

	bool m_WasPlayBGM = false;
	int  m_PreviousBeatIndex = 0;
    
protected:

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

	void RegisterAudio();
    // 効果音再生関数
    // seName : AudioConfigに登録した名前
    // Game.hのAudioManagerを通じて再生する
	void PlaySE(std::string seName, std::optional<float> overrideVolume);
public:
	static SceneRelationData m_RelationData;
	// コンストラクタとデストラクタ
	Scene() = default;
	Scene(Camera& cam);
	virtual ~Scene() = default;

	//---------------------------------
	// 			ループ内の処理
	//		 純粋仮想関数として定義
	//---------------------------------

	virtual void Initialize()		= 0;	// 初期化処理
	virtual void Update(float tick) = 0;	// 更新処理
	virtual void Draw();					// 描画処理
	virtual void Finalize()			= 0;	// 解放処理

	
	Camera& GetCamera() {
		return m_Camera;
	}

	// そのシーンのオブジェクトを定義
	vector<pShared<Object>> GetSceneObjects();
	void SetTheme(const pShared<Theme>& theme) {
		m_Theme = theme;
	}
	pShared<Theme> GetTheme() const {
		return m_Theme;
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

	static void ClearTimerList()
	{
        m_TimerList.clear();
	}

	//================================
	// オブジェクト管理
	//================================
	void DeleteObject(const pShared<Object>& pt); // オブジェクトを削除する
	void DeleteAllObject(); // オブジェクトをすべて削除する

	// オブジェクトを追加する
	template<class T, class... Args>
	pShared<T> AddObject(Args&&... args)
	{
		static_assert(std::is_base_of_v<Object, T>, "TがObjectを継承していない");
		static_assert(!std::is_abstract_v<T>, "Tが抽象クラスだった");

		// コンストラクタ引数を完全転送して unique_ptrを作成
		pShared<T> up;
		if constexpr (sizeof...(Args) == 0) {
			up = std::make_shared<T>();
		}
		else {
			up = std::make_shared<T>(std::forward<Args>(args)...);
		}

		m_MySceneObjects.emplace_back(up);
		up->Initialize(); // 初期化
		return up;
	}

	// オブジェクトを取得する
	template<class T>
	vector<pShared<T>> GetObjects()
	{
		static_assert(std::is_base_of_v<Object, T>, L"TがObjectを継承していない");

		vector<pShared<T>> res;
		for (const auto& o : m_MySceneObjects) {
			// dynamic_castで型をチェック
			if (!o) {
				continue;
			}
			if (auto derivedObj = std::dynamic_pointer_cast<T>(o)) {
				res.emplace_back(std::move(derivedObj));
			}
		}
		return res;
	}
};


