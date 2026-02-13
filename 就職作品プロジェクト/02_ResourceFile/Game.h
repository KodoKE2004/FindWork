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
	static std::unique_ptr<Game>		 m_pInstance;					// ゲームのインスタンス
	static uint64_t						 m_DrawFrameCounter;			// Draw 呼び出しのフレーム番号
	std::shared_ptr<Scene>				 m_SceneCurrent;				// 現在のシーン
    std::shared_ptr<Scene>				 m_SceneNext;					// 次のシーン
	std::unique_ptr<Input>				 m_Input;						// 入力管理
	std::shared_ptr<Camera>				 m_Camera;						// カメラ
    std::vector<std::shared_ptr<TransitionBase>> m_TransitionTexture;	// トランジション用テクスチャ
    std::shared_ptr<Theme>				 m_Theme;						// テーマ管理
    std::vector<std::shared_ptr<Scene>>	 m_SceneList;					// シーンスタック
    DirectX::SimpleMath::Vector2		 m_PreviewMousePos;				// デバッグ用ビュー行列
	std::shared_ptr<Audio>				 m_BgmAudio;					// BGM
	PlayParams							 m_BgmParams;

#ifdef _DEBUG
#endif

	//================================
	//	   ゲームを支えるマネージャー達
	//================================
	std::shared_ptr<MeshManager>		 m_MeshManager;		// シーンで扱うメッシュ
	std::shared_ptr<TextureManager>		 m_TextureManager;	// ゲームで扱う画像
	std::shared_ptr<ShaderManager>		 m_ShaderManager;	// シェーダーをまとめたもの
	std::shared_ptr<AudioManager>		 m_AudioManager;	// オーディオマネージャー

    static void InitializeTransitionCSV();					// トランジションCSVの初期化
    static void FinalizeTransitionCSV();					// トランジションCSVの終了処理
	static int	 m_DifficultyStageInterval;
	static float m_BaseBpmIncreasePerDifficulty;
	static int	 m_SpeedUpStageInterval;
	static float m_SpeedUpBpmIncrease;

	// テンポ制御
	static RhythmBeat m_RhythmBeat;
	static bool		  m_isTickCount;

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	Game();
	~Game() = default;
	//================================
	//			ループ内の処理
	//================================
	
	static void Initialize();		// ゲームの初期化
	static void Update(float tick);	// ゲームの更新
	static void Draw();				// ゲームの描画
	static void Finalize();			// ゲームの終了処理

	// 現在のシーンを設定
    static void SetSceneCurrent(std::shared_ptr<Scene> newScene);
    static void SetSceneNext(std::shared_ptr<Scene> newScene);

	void SetTheme(const std::shared_ptr<Theme>& theme);

    // TransitionTextureをTransSceneと連携
	void SetTransitionTexture(std::vector<std::shared_ptr<TransitionBase>> tex) {
		m_TransitionTexture = tex;
    }
	void AddTransitionTexture(const std::shared_ptr<TransitionBase>& tex) {
		m_TransitionTexture.emplace_back(tex);
    }
	void ClearTransitionTexture() {
		m_TransitionTexture.clear();
    }

	std::vector<std::shared_ptr<TransitionBase>> GetTransitionTexture() const;
	std::shared_ptr<Theme>			GetTheme() ; 

    //===============================
	//			シーンの関連群
	//===============================
	void					ScenePush(std::shared_ptr<Scene> newScene);
	std::shared_ptr<Scene>	ScenePop();
	size_t					GetSceneStackSize() const;
	
	static Game&				   GetInstance();	
	static std::shared_ptr<Scene>  GetCurrentScene();
	Camera&						   GetCamera();
	static void					   SetBgmBpm(float bpm);
	static float				   GetBgmBpm();
	static RhythmBeat&			   GetRhythmBeat() {
		return m_RhythmBeat; 
	}
	static uint64_t			GetDrawFrameCounter() {
		return m_DrawFrameCounter;
	}
	static void				SetDifficultyStageInterval(int interval);
	static int				GetDifficultyStageInterval();
	static void				SetBaseBpmIncreasePerDifficulty(float bpmIncrease);
	static float			GetBaseBpmIncreasePerDifficulty();
	static void				SetSpeedUpStageInterval(int interval);
	static int				GetSpeedUpStageInterval();
	static void				SetSpeedUpBpmIncrease(float bpmIncrease);
	static float			GetSpeedUpBpmIncrease();
	static void				PlayBgm();
	static void				StopBgm();
	static void				SetIsTickCount(bool isTick) {m_isTickCount = isTick; }
	static bool				IsTickCount() {
		return m_isTickCount;
	}

    //================================
	//		  マネージャーの取得
    //================================
	[[nodiscard]] operator MeshManager*    () const { return m_MeshManager.get();	}
	[[nodiscard]] operator TextureManager* () const { return m_TextureManager.get();}
	[[nodiscard]] operator ShaderManager*  () const { return m_ShaderManager.get(); }
	[[nodiscard]] operator AudioManager*   () const { return m_AudioManager.get();	}

	static void RegistDebugObject();
	static void RegistDebugFunction(){}

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

	Scene::ClearTimerList();

	auto scene     = std::make_shared<TransScene>(instance.GetCamera());
	auto sceneNext = std::make_shared<T>(instance.GetCamera(), std::forward<Args>(args)...);
    auto sceneCurrent = instance.GetCurrentScene();

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

	Scene::ClearTimerList();

    // 現在のシーン
    auto scene = std::make_shared<TransScene>(instance.GetCamera());
    auto sceneNext = instance.ScenePop();

	if (!sceneNext) {
		return;
	}

	scene->SetOldScene(instance.GetCurrentScene());
    scene->SetRelationData(instance.GetCurrentScene()->GetRelationData());
    scene->SetNextScene(sceneNext);
	scene->SetTransitionParam(state);
	scene->SetStep(STEP::START);
	scene->Initialize();

    instance.SetSceneCurrent(scene);
}

inline std::vector<std::shared_ptr<TransitionBase>> Game::GetTransitionTexture() const
{
	return m_TransitionTexture;
}

inline void Game::ScenePush(std::shared_ptr<Scene> newScene)
{
    if (newScene){
		m_SceneList.push_back(newScene);
	}
}

inline std::shared_ptr<Scene> Game::ScenePop()
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
