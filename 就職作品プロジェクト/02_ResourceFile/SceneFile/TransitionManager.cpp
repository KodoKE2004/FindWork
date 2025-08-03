#include "TransitionManager.h"

void TransitionManager::Update()
{
	if (m_Finished) return;
	m_Timer += 0.016f; // ‚±‚±‚Å‚Í‰¼‚É1ƒtƒŒ[ƒ€‚ð16ms‚Æ‚·‚é
	if (m_Timer >= m_Duration) {
		m_Finished = true;
	}
}

void TransitionManager::DrawOverlay()
{
	
}

bool TransitionManager::IsFinished()
{
	return m_Finished;
}
