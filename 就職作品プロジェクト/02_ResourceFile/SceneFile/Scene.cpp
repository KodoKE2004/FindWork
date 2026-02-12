#include "Scene.h"
#include "Game.h"
#include <algorithm>

SceneRelationData Scene::m_RelationData;
std::vector<float*> Scene::m_TimerList;

std::vector<std::shared_ptr<Object>> Scene::GetSceneObjects()
{
	std::vector<std::shared_ptr<Object>> objects = m_MySceneObjects;
	if (m_Theme)
	{
		const auto themePtr = m_Theme.get();
		const bool alreadyAdded = std::any_of(objects.begin(), objects.end(),
			[themePtr](const std::shared_ptr<Object>& obj)
			{
				return obj.get() == themePtr;
			});
		if (!alreadyAdded)
		{
			objects.emplace_back(m_Theme);
		}
	}
	return objects;
}

void Scene::Draw()
{
	for (auto object : m_MySceneObjects)
	{
		if (object) {
			object->Draw();
		}
	}
}

void Scene::PlaySE(std::string seName, std::optional<float> overrideVolume)
{
	// SEの再生
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

Scene::Scene(std::shared_ptr<Camera> cam) : m_Camera(cam)
{
}

void Scene::DeleteObject(const std::shared_ptr<Object>& pt)
{
	if (pt == nullptr) return;

	auto& objs = m_MySceneObjects;
	const auto raw = pt.get();
	auto it = std::find_if(objs.begin(), objs.end(),
		[raw](const std::shared_ptr<Object>& up) {
			return up.get() == raw;
		});

	if (it != objs.end())
	{
		(*it)->Finalize();
		objs.erase(it);
	}

}

void Scene::DeleteAllObject()
{
	// オブジェクト終了処理
	for (auto& o : m_MySceneObjects)
	{
		if (!o) {
			o->Finalize();
		}
	}

	m_MySceneObjects.clear();
	m_MySceneObjects.shrink_to_fit();
}
