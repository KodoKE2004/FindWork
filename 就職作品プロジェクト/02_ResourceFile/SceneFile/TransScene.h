#pragma once
#include "Scene.h"
#include "Square.h"
#include "Application.h"

#include "RenderTarget.h"
#include "TransitionBase.h"
#include "SnapshotOverlay.h"

#include <memory>

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
	std::shared_ptr<Scene> m_SceneOld ;
	std::shared_ptr<Scene> m_SceneNext;

	// オフスクリーン用
	std::unique_ptr<RenderTarget> m_RenderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NextSceneSRV;

	SnapshotOverlay* m_OverlayOld  = nullptr;
	SnapshotOverlay* m_OverlayNext = nullptr;

    // タイマー関連
	float m_Timer	 = 0.0f;
	float m_Duration = 0.0f;

	// 遷移演出のパラメータ
	SceneTransitionParam m_transParam{};

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
    ~TransScene();

	void Initialize()		override;
	void Update(float tick)	override;
	void Finalize()			override;

	SCENE_NO GetSceneNo() const override { return SCENE_NO::TRANSITION; }
	/// <summary>
	/// 次のシーンの一フレーム目を描画する
	/// </summary>
	void DrawNextScene();

	void SetTransitionParam(SceneTransitionParam param) 
	{	
		m_transParam  = param;  
	}

	void SetOldScene (const std::shared_ptr<Scene>& sceneOld)     { m_SceneOld  = sceneOld;  }
	void SetNextScene(const std::shared_ptr<Scene>& sceneNext)    { m_SceneNext = sceneNext; }
	void SetTransMode(TRANS_MODE mode)     { m_TransMode = mode;	  }
	void SetStep(STEP step)				   { m_Step		 = step;	  }
	void SetTimer(float timer)			   { m_Timer	 = timer;	  }

	// 設定した時間に到達したかどうか
	bool isOverClock();  

};
