#include "SceneTrans.h"

std::unique_ptr<Scene> SceneTrans::m_NextScene  = nullptr;	// 次のシーン
float				   SceneTrans::m_Timer      = 0.0f	;	// タイマー
float				   SceneTrans::m_Duration   = 1.0f	;	// 遷移時間
SWITCH				   SceneTrans::m_Behavior   = OFF	;	// 遷移が完了したかどうか
TRANS_MODE			   SceneTrans::m_TransMode  = FADE	;	// 遷移の仕方

void SceneTrans::Initialize(TRANS_MODE mode)
{
	m_Timer = 0.0f;

}

void SceneTrans::Draw()
{
	if      (m_Behavior == OFF)  { return; }
	switch (m_TransMode)
	{
	case FADE:
	{



	}
	break;


	}


}
void SceneTrans::SetTransition(SWITCH setSwitch)
{
	m_Behavior = setSwitch;
}

SWITCH SceneTrans::IsFinished()
{
	return m_Behavior;
}

void SceneTrans::StartTransition(TRANS_MODE mode, float duration)
{
	m_Duration  = duration;
	m_TransMode = mode;
	m_Behavior  = START;
	m_Timer     = 0.0f;
	switch (m_TransMode)
	{
	case FADE:
	{
		
	}
	break;


	}
}
