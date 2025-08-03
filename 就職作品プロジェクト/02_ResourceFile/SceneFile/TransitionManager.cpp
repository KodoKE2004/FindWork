#include "TransitionManager.h"

void TransitionManager::Update()
{

	switch (m_Behevior)
	{
	case OFF:						  return;
	case FINISH:	m_Behevior = OFF; return;
	}

	m_Timer += 0.016f; // ‚±‚±‚Å‚Í‰¼‚É1ƒtƒŒ[ƒ€‚ð16ms‚Æ‚·‚é
	if (m_Timer >= m_Duration) {
		m_Behevior = FINISH;
	}
}

void TransitionManager::DrawOverlay()
{
	
}

void TransitionManager::SetTransition(SWITCH setSwitch)
{
	m_Behevior = setSwitch;
}

SWITCH TransitionManager::IsFinished()
{
	return m_Behevior;
}
