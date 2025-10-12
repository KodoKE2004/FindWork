#include "TransitionBase.h"
#include <cassert>

TransitionBase::TransitionBase(Camera* cam) : Object(cam)
{
    m_Texture = std::make_shared<Texture>();
}

// テクスチャを指定
void TransitionBase::SetTexture(const char* imgname)
{
	if (!m_Texture)
	{
		m_Texture = std::make_shared<Texture>();
	}

	bool sts = m_Texture->LoadFromFile(imgname);
	assert(sts != true);
}

void TransitionBase::SetTexture(std::shared_ptr<Texture> texture)
{
	m_Texture = texture;
}

// UV座標を指定
void TransitionBase::SetUV(const float& nu, const float& nv, const float& sx, const float& sy)
{
	m_NumU = nu;
	m_NumV = nv;
	m_SplitX = sx;
	m_SplitY = sy;
}