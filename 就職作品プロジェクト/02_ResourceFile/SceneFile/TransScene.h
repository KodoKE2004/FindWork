#pragma once
#include "Scene.h"
#include "../Texture2D.h"
#include "../Application.h"
enum STEP
{
	OFF,
	START,
	DOING,
	FINISH,
};

enum TRANS_MODE
{
	FADE,
};


class TransScene : public Scene
{
private:
	Scene* m_SceneOld;
	Scene* m_SceneNext;

	const float m_Counter = Application::GetDeltaTime();
	float m_Timer	 = 0.0f;
	float m_Duration = 0.0f;

	float m_Alpha = 0.0f;
	float m_AlphaValue = 0.0f;

	bool m_isChange = false;

	STEP m_Step;
	TRANS_MODE m_TransMode;

	Texture2D* m_Texture;

public:
	void Initialize()	override;
	void Update()		override;
	void Finalize()		override;

	void SetDuration(float duration) { m_Duration = duration; }
	void SetOldScene(Scene* sceneOld) {
		m_SceneOld = sceneOld;
	}
	void SetNextScene(Scene* sceneNext) {
		m_SceneNext = sceneNext;
	}
	void SetTransMode(TRANS_MODE mode) {
		m_TransMode = mode;
	}
	void SetStep(STEP step) {
		m_Step = step;
	}

	bool isOverClock();
	
public:	//		フェード
	void FADE_IN();
	void FADE_OUT();

};

