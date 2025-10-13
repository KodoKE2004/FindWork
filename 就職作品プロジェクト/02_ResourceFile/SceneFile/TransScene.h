#pragma once
#include "Scene.h"
#include "../Texture2D.h"
#include "../Application.h"
#include "../RenderTarget.h"
#include "Transition/Fade.h"
#include "Transition/TransitionBase.h"
#include "Transition/SnapshotOverlay.h"

enum class STEP
{
	OFF,
	START,
	DOING,
	FINISH,
};

enum class TRANS_MODE
{
	FADE,
	NUM
};


class TransScene : public Scene
{
private:
	// シーンの前後 
	Scene* m_SceneOld;
	Scene* m_SceneNext;

	// オフスクリーン用
	std::unique_ptr<RenderTarget> m_RenderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NextSceneSRV;
	SnapshotOverlay* m_Overlay = nullptr;

    // タイマー関連
	float m_Timer	 = 0.0f;
	float m_Duration = 0.0f;

    // トランジション関連
	float m_Alpha = 0.0f;
	float m_AlphaValue = 0.0f;


	bool m_isChange = false;
	STEP m_Step;
	TRANS_MODE m_TransMode;

	std::shared_ptr<TransitionBase> m_TransitionTexture;

public:
	void Initialize()	override;
	void Update(float tick)	override;
	void Finalize()		override;

	/// <summary>
	/// 次のシーンの一フレーム目を描画する
	/// </summary>
	void DrawNextScene();

	void SetDuration(float duration)    { m_Duration = duration;	}
	void SetOldScene(Scene* sceneOld)   { m_SceneOld = sceneOld;	}
	void SetNextScene(Scene* sceneNext) { m_SceneNext = sceneNext;	}
	void SetTransMode(TRANS_MODE mode)  { m_TransMode = mode;		}
	void SetStep(STEP step)				{m_Step = step;				}

	bool isOverClock();
	
public:	//		フェード
	void FADE_IN();
	void FADE_OUT();

};

