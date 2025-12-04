#include <cassert>
#include <cmath>
#include "TransitionBase.h"
#include "Collider.h"

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
	assert(sts == true);
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

float TransitionBase::EvaluateEasing(const SceneTransitionParam& param, float t)
{
	switch (param.easing)
	{
	default:							return t;
    case EASING_TYPE::IN_SINE:			return Math::Easing::EaseInSine(t);
    case EASING_TYPE::IN_QUAD:			return Math::Easing::EaseInQuad(t);
    case EASING_TYPE::IN_CUBIC:			return Math::Easing::EaseInCubic(t);
    case EASING_TYPE::IN_QUINT:			return Math::Easing::EaseInQuint(t);
    case EASING_TYPE::IN_QUART:			return Math::Easing::EaseInQuart(t);
    case EASING_TYPE::IN_EXPO:			return Math::Easing::EaseInExpo(t);
    case EASING_TYPE::IN_CIRC:			return Math::Easing::EaseInCirc(t);
    case EASING_TYPE::IN_BACK:			return Math::Easing::EaseInBack(t);
    case EASING_TYPE::IN_BOUNCE:		return Math::Easing::EaseInBounce(t);
    case EASING_TYPE::IN_ELASTIC:		return Math::Easing::EaseInElastic(t);
    case EASING_TYPE::OUT_SINE:			return Math::Easing::EaseOutSine(t);
    case EASING_TYPE::OUT_QUAD:			return Math::Easing::EaseOutQuad(t);
    case EASING_TYPE::OUT_CUBIC:		return Math::Easing::EaseOutCubic(t);
    case EASING_TYPE::OUT_QUINT:		return Math::Easing::EaseOutQuint(t);
    case EASING_TYPE::OUT_QUART:		return Math::Easing::EaseOutQuart(t);
    case EASING_TYPE::OUT_CIRC:			return Math::Easing::EaseOutCirc(t);
    case EASING_TYPE::OUT_BACK:			return Math::Easing::EaseOutBack(t);
    case EASING_TYPE::OUT_ElASTIC:		return Math::Easing::EaseOutElastic(t);
    case EASING_TYPE::OUT_BOUNCE:		return Math::Easing::EaseOutBounce(t);
	case EASING_TYPE::OUT_EXPO:			return Math::Easing::EaseOutExpo(t);
	case EASING_TYPE::IN_OUT_SINE:		return Math::Easing::EaseInOutSine(t);
	case EASING_TYPE::IN_OUT_QUAD:		return Math::Easing::EaseInOutQuad(t);
	case EASING_TYPE::IN_OUT_CUBIC:		return Math::Easing::EaseInOutCubic(t);
	case EASING_TYPE::IN_OUT_QUINT:		return Math::Easing::EaseInOutQuint(t);
	case EASING_TYPE::IN_OUT_QUART:		return Math::Easing::EaseInOutQuart(t);
	case EASING_TYPE::IN_OUT_CIRC:		return Math::Easing::EaseInOutCirc(t);
	case EASING_TYPE::IN_OUT_BACK:		return Math::Easing::EaseInOutBack(t);
	case EASING_TYPE::IN_OUT_ELASTIC:	return Math::Easing::EaseInOutElastic(t);
	case EASING_TYPE::IN_OUT_BOUNCE:	return Math::Easing::EaseInOutBounce(t);
    case EASING_TYPE::IN_OUT_EXPO:		return Math::Easing::EaseInOutExpo(t);

	}
}

void TransitionBase::ApplyPhaseSetting(PHASE phase)
{
	const auto& param = GetParamForPhase(phase);
	m_Duration = max(param.duration, 0.0001f);
	SetTransMode(param.mode);
	m_Phase = phase;
}
