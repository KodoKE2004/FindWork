#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <cstdint>
#include <Debug.hpp>

#include "Scene.h"
#include "TransScene.h"
#include "TransitionBase.h"
#include "Theme.h"
#include "input.h"
#include "RhythmBeat.h"

#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "AudioManager.h"

class Game
{
private:
	// Game は「シーン単体で持ちにくい共有資源」を束ねる中核クラス。
	// ここではシーン本体のロジックではなく、進行の土台（Scene/各Manager/リズム）を管理する。
	static pUnique<Game>			m_pInstance;				// シングルトン本体
	static uint64_t					m_DrawFrameCounter;			// Draw 呼び出し回数（デバッグ表示や計測の基準）
	pShared<Scene>					m_SceneCurrent;				// 現在進行中のシーン
	pShared<Scene>					m_SceneNext;				// 遷移先シーン（トランジション中に保持）
	pUnique<Input>					m_Input;					// 入力管理
	pShared<Camera>					m_Camera;					// カメラ
	vector<pShared<TransitionBase>> m_TransitionTexture;		// トランジション描画を Scene 外から重ねるためのオーバーレイ群
	pShared<Theme>					m_Theme;					// テーマ管理
	vector<pShared<Scene>>			m_SceneList;				// シーンスタック（戻る遷移のために退避）
	DirectX::SimpleMath::Vector2	m_PreviewMousePos;			// デバッグ用ビュー行列
	pShared<Audio>					m_BgmAudio;					// BGM
	PlayParams						m_BgmParams;

#ifdef _DEBUG
#endif

	//================================
	//	   ゲームを支えるマネージャー達
	//================================
	pShared<MeshManager>		 m_MeshManager;		// シーンで扱うメッシュ
	pShared<TextureManager>		 m_TextureManager;	// ゲームで扱う画像
	pShared<ShaderManager>		 m_ShaderManager;	// シェーダーをまとめたもの
	pShared<AudioManager>		 m_AudioManager;	// オーディオマネージャー

    static void InitializeTransitionCSV();					// トランジションCSVの初期化
    static void FinalizeTransitionCSV();					// トランジションCSVの終了処理
	static int	 m_DifficultyStageInterval;
	static float m_BaseBpmIncreasePerDifficulty;
	static int	 m_SpeedUpStageInterval;
	static float m_SpeedUpBpmIncrease;

	// テンポ制御（Scene を跨いで同一基準の時間軸を使うため Game 側で保持）
	static RhythmBeat m_RhythmBeat;
	// 「Update を止める」のではなく「ゲーム進行時間(Tick)だけ止める」ためのフラグ。
	// 演出や遷移UIを動かしながら、判定系の時間だけ止めたい場面で使う。
	static bool		  m_isTickCount;
	static bool       s_HasFirstGameSceneWaitInitialized;

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	Game();
	~Game() = default;
	//================================
	//			ループ内の処理
	//================================
	
	// アプリ起動時に一度だけ呼ばれ、各Manager・初期Scene・描画基盤を組み立てる。
	static void Initialize();		// ゲームの初期化
	// 1フレーム単位で Scene / Camera / Audio 等の全体更新を進める。
	static void Update(float tick);	// ゲームの更新
	// Scene 描画 + 遷移オーバーレイ + デバッグUI の合成を担当する。
	static void Draw();				// ゲームの描画
	// リソース解放順を管理し、Scene 側が握る共有資源もここで確実に終了させる。
	static void Finalize();			// ゲームの終了処理

	// 現在のシーンを設定
	static void SetSceneCurrent(pShared<Scene> newScene);	// 実行中シーンを切り替える
	static void SetSceneNext(pShared<Scene> newScene);		// 遷移先シーンを設定する
	static void SceneStackClear();
	void SetTheme(const pShared<Theme>& theme);

    // TransitionTextureをTransSceneと連携
	void SetTransitionTexture(vector<pShared<TransitionBase>> tex) {
		m_TransitionTexture = tex;
    }
	void AddTransitionTexture(const pShared<TransitionBase>& tex) {
		m_TransitionTexture.emplace_back(tex);
    }
	void ClearTransitionTexture() {
		m_TransitionTexture.clear();
    }

	vector<pShared<TransitionBase>> GetTransitionTexture() const;
	pShared<Theme>			GetTheme() ;

    //===============================
	//			シーンの関連群
	//===============================
	void					ScenePush(pShared<Scene> newScene);
	pShared<Scene>	ScenePop();
	size_t					GetSceneStackSize() const;
	
	static Game&			GetInstance();	
	static pShared<Scene>	GetCurrentScene();
	Camera&					GetCamera();
	static void				SetBgmBpm(float bpm);
	static float			GetBgmBpm();
	// Scene 共通の時間基準。待機シーン/ゲームシーンで同じ拍進行を参照する。
	static RhythmBeat&		GetRhythmBeat() {
		return m_RhythmBeat; 
	}
	static uint64_t			GetDrawFrameCounter() {
		return m_DrawFrameCounter;
	}
	static void	 SetDifficultyStageInterval(int interval);
	static int	 GetDifficultyStageInterval();
	static void	 SetBaseBpmIncreasePerDifficulty(float bpmIncrease);
	static float GetBaseBpmIncreasePerDifficulty();
	static void	 SetSpeedUpStageInterval(int interval);
	static int	 GetSpeedUpStageInterval();
	static void	 SetSpeedUpBpmIncrease(float bpmIncrease);
	static float GetSpeedUpBpmIncrease();
	static void	 PlayBgm();
	static void	 StopBgm();
	static void	 SetIsTickCount(bool isTick) {m_isTickCount = isTick; }
	static bool	 IsTickCount() {
		return m_isTickCount;
	}
	static bool  HasFirstGameSceneWaitInitialized() {
        return s_HasFirstGameSceneWaitInitialized;
	}
	static void  SetHasFirstGameSceneWaitInitialized(bool initialized) {
        s_HasFirstGameSceneWaitInitialized = initialized;
	}

    //================================
	//		  マネージャーの取得
    //================================
	[[nodiscard]] operator MeshManager*    () const { return m_MeshManager.get();	}
	[[nodiscard]] operator TextureManager* () const { return m_TextureManager.get();}
	[[nodiscard]] operator ShaderManager*  () const { return m_ShaderManager.get(); }
	[[nodiscard]] operator AudioManager*   () const { return m_AudioManager.get();	}

	static void ResistDebugObject();
	static void ResistDebugFunction(){}

};

//================================
//			グローバル関数
// 　　　インスタンスの取得を簡易化
//================================

//================================
//	  シーンを遷移するテンプレート関数
//================================
template<class T,class... Args>
void ChangeScenePush(SceneTransitionParam& state, Args&&... args)
{
    auto& instance = Game::GetInstance();

	// テンプレートなので
	// 既定がSceneでなければエラー
	// T が Scene を継承していること、かつ抽象クラスでないことをチェック
	static_assert(std::is_base_of_v<Scene, T>, "T は Scene を継承している必要があります");
	static_assert(!std::is_abstract_v<T>, "T は抽象クラスではいけません");
	static_assert(std::is_constructible_v<T, Camera&, Args...>, "T は (Camera&, Args...) で構築できる必要があります");

	Debug::Log("[[検出]] シーンのPush");

	// Scene 遷移時に前シーン由来のタイマーが残ると誤作動するため、全体でクリアする。
	Scene::ClearTimerList();

	// 実際のシーン切り替え前に TransScene を挟み、old/new の橋渡しを行う。
	auto scene     = std::make_shared<TransScene>(instance.GetCamera());
	auto sceneNext = std::make_shared<T>(instance.GetCamera(), std::forward<Args>(args)...);
    auto sceneCurrent = instance.GetCurrentScene();

    scene->m_RelationData.oldScene  = sceneCurrent->GetSceneNo();
    scene->m_RelationData.nextScene = sceneNext->GetSceneNo();
    instance.ScenePush(sceneCurrent);
    instance.SetSceneNext(sceneNext);

	scene->SetOldScene(sceneCurrent);
	scene->SetNextScene(sceneNext);
	scene->SetTransitionParam(state);
	scene->SetStep(STEP::START);
	scene->Initialize();

	instance.SetSceneCurrent(scene);

}

// 一つ前のシーンに戻る
inline void ChangeScenePop(SceneTransitionParam& state)
{
    auto& instance = Game::GetInstance();

	if (instance.GetSceneStackSize() == 0) {
		std::string_view msg = "[[警告]] シーンスタックが空です";
		Debug::Log(msg, MESSAGE_COLOR::YELLOW);
		return;
	}

	Debug::Log("[[検出]] シーンのPop");
	// Pop 遷移時も同様に、前シーンの残タイマーを持ち越さない。
	Scene::ClearTimerList();

	// 現在のシーン
	auto scene = std::make_shared<TransScene>(instance.GetCamera());	// 中継用シーン
    auto sceneNext = instance.ScenePop();

	if (!sceneNext) {
		return;
	}
	scene->m_RelationData.oldScene  = scene->m_RelationData.nextScene;
    scene->m_RelationData.nextScene = sceneNext->GetSceneNo();
	scene->SetOldScene(instance.GetCurrentScene());
    scene->SetRelationData(instance.GetCurrentScene()->GetRelationData());
    scene->SetNextScene(sceneNext);
	scene->SetTransitionParam(state);
	scene->SetStep(STEP::START);
	scene->Initialize();

    instance.SetSceneCurrent(scene);
}

inline vector<pShared<TransitionBase>> Game::GetTransitionTexture() const
{
	return m_TransitionTexture;
}

inline void Game::ScenePush(pShared<Scene> newScene)
{
    if (newScene){
		m_SceneList.push_back(newScene);
	}
}

inline pShared<Scene> Game::ScenePop()
{
	if(m_SceneList.empty()) return nullptr;
	
    auto scene = m_SceneList.back();
    m_SceneList.pop_back();

	return scene;
}

inline size_t Game::GetSceneStackSize() const
{
	return m_SceneList.size();
}
