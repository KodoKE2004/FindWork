#include "Transition.h"

void SceneTrans::Update()
{
	m_Timer += 0.016f; // ‚±‚±‚Å‚Í‰¼‚É1ƒtƒŒ[ƒ€‚ð16ms‚Æ‚·‚é
	if (m_Timer >= m_Duration) {
		m_Behevior = FINISH;
	}
}

void SceneTrans::DrawOverlay()
{
	
}

void SceneTrans::StartTransition(TRANS_MODE mode)
{
	if(m_Behevior == FINISH) return ;

	switch(m_TransMode)
	{
	case FADE:
	{
		
		
		
	}
	break;


	}

}

void SceneTrans::SetTransition(SWITCH setSwitch)
{
	m_Behevior = setSwitch;
}

SWITCH SceneTrans::IsFinished()
{
	return m_Behevior;
}
