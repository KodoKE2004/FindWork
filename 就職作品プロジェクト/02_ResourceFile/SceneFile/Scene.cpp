#include "Scene.h"
#include "Game.h"
#include <algorithm>

SceneRelationData Scene::m_RelationData;
std::vector<float*> Scene::m_TimerList;

void Scene::PlaySE(std::string seName, std::optional<float> overrideVolume)
{
	// SE‚ÌÄ¶
	if (AudioManager* audioMgr = Game::GetInstance())
	{
		PlayParams params{};
		bool hasConfig = false;

		if (auto it = m_AudioList.find(seName); it != m_AudioList.end())
		{
			params = it->second.params;
			if (it->second.loop)
			{
				params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
			}
			hasConfig = true;;
		}
		else
		{
			params.volume = DEFAULT_VOLUME;
		}

		if (overrideVolume.has_value())
		{
			params.volume = max(0.0f, overrideVolume.value());
		}
		else if (!hasConfig)
		{
			params.volume = DEFAULT_VOLUME;
		}

		audioMgr->Play(seName, params);
	}
}
