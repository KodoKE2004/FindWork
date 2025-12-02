#include "Scene.h"
#include "Game.h"

SceneRelationData Scene::m_RelationData;
std::vector<float*> Scene::m_TimerList;

void Scene::PlaySE(std::string seName)
{
	
	// SE‚ÌÄ¶
	if (AudioManager* audioMgr = Game::GetInstance())
	{
		if (auto it = m_AudioList.find("seName"); it != m_AudioList.end())
		{
			auto params = it->second.params;
			if (it->second.loop)
			{
				params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
			}
			audioMgr->Play("seName", params);
		}
		else
		{
			audioMgr->Play("seName");
		}
	}
}
