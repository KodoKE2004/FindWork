#pragma once
#include "Scene.h"
#include "Square.h"
#include "Application.h"

#include "RenderTarget.h"
#include "Fade.h"
#include "Wipe.h"
#include "TransitionBase.h"
#include "SnapshotOverlay.h"

// 列挙型PHASEの進行状況の監視
// OFF    : トランジション処理を行わない
// START  : トランジション処理を開始する
// DOING  : トランジション処理中
// FINISH : トランジション処理が終了した
enum class STEP
{
	OFF,
	START,
	DOING,
	FINISH,
	NUM
};

class TransScene : public Scene
{
private:
	// シーンの前後 
	Scene* m_SceneOld  = nullptr;
	Scene* m_SceneNext = nullptr;

	// オフスクリーン用
	std::unique_ptr<RenderTarget> m_RenderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NextSceneSRV;

	SnapshotOverlay* m_OverlayOld  = nullptr;
	SnapshotOverlay* m_OverlayNext = nullptr;

    // タイマー関連
	float m_Timer	 = 0.0f;
	float m_Duration = 0.0f;

    // トランジション関連
	float m_Alpha	   = 0.0f;
	float m_AlphaValue = 0.0f;

	bool m_isChange		    = false;

	STEP m_Step;

	TRANS_MODE m_TransMode;

	std::shared_ptr<TransitionBase> m_TransitionTexture;

public:
    // コンストラクタとデストラクタ

	TransScene();
    ~TransScene()			override = default;

	void Initialize()		override;
	void Update(float tick)	override;
	void Finalize()			override;

	SCENE_NO GetSceneNo() const override { return SCENE_NO::TRANSITION; }
	/// <summary>
	/// 次のシーンの一フレーム目を描画する
	/// </summary>
	void DrawNextScene();

	void SetTransitionTick(float duration) { m_Duration  = duration;  }
	void SetOldScene(Scene* sceneOld)      { m_SceneOld  = sceneOld;  }
	void SetNextScene(Scene* sceneNext)    { m_SceneNext = sceneNext; }
	void SetTransMode(TRANS_MODE mode)     { m_TransMode = mode;	  }
	void SetStep(STEP step)				   { m_Step		 = step;	  }
	void SetTimer(float timer)			   { m_Timer	 = timer;	  }

	// 設定した時間に到達したかどうか
	bool isOverClock();  

};
