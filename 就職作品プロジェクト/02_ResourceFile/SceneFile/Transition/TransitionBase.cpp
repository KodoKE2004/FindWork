#include <cassert>
#include <cmath>
#include <imgui.h>
#include "TransitionBase.h"
#include "Collider.h"

SceneTransitionParam TitleToWait{};
SceneTransitionParam WaitToGame{};
SceneTransitionParam GameToWait{};
SceneTransitionParam WaitToResult{};
SceneTransitionParam ResultToTitle{};
SceneTransitionParam ResultToGame{};

namespace TransGui
{
	constexpr int kTransModeCount   = static_cast<int>(TRANS_MODE::NUM);
	constexpr int kEaseingTypeCount = static_cast<int>(EASING_TYPE::NUM);

	const char* kTransModeLabels[kTransModeCount] =
	{
		"Fade",
		"Wipe Left -> Right",
		"Wipe Right -> Left",
		"Wipe Top -> Bottom",
		"Wipe Bottom -> Top",
		"Zoom In",
		"Zoom Out",
	};

	const char* kEasingLabels[kEaseingTypeCount] =
	{
		"None",
		"In Sine",
		"In Quad",
		"In Cubic",
		"In Quint",
		"In Quart",
		"In Expo",
		"In Circ",
		"In Back",
		"In Bounce",
		"In Elastic",
		"Out Sine",
		"Out Quad",
		"Out Cubic",
		"Out Quint",
		"Out Quart",
		"Out Circ",
		"Out Back",
		"Out Elastic",
		"Out Bounce",
		"Out Expo",
		"InOut Sine",
		"InOut Quad",
		"InOut Cubic",
		"InOut Quint",
		"InOut Quart",
		"InOut Circ",
		"InOut Back",
		"InOut Elastic",
		"InOut Bounce",
		"InOut Expo",
	};

	void DrawTransitionStateUI(const char* label, SceneTransitionParam& param)
	{
		ImGui::PushID(label);
		if (ImGui::TreeNode(label))
		{
			int modeIndex = param.ModeAsIndex();
			if (ImGui::Combo("Mode", &modeIndex, kTransModeLabels, kTransModeCount))
			{
				param.mode = static_cast<TRANS_MODE>(modeIndex);
			}

			ImGui::DragFloat("Duration", &param.duration, 0.01f, 0.0f, 30.0f, "%.2f sec");

			int easingIndex = param.EasingAsIndex();
			if (ImGui::Combo("Easing", &easingIndex, kEasingLabels, kEaseingTypeCount))
			{
				param.easing = static_cast<EASING_TYPE>(easingIndex);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

}



TransitionBase::TransitionBase(Camera* cam) : Object(cam)
{
    m_Texture = std::make_shared<Texture>();
	m_TransMode = TRANS_MODE::NUM;
    m_Phase = PHASE::FINISH;
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
	const auto& param = m_transParam;
	m_Duration = max(param.duration, 0.0001f);
	SetTransMode(param.mode);
	m_Phase = phase;
}

void DrawTransitionStateGUI()
{
	if (ImGui::Begin("Scene Transition Settings"))
	{
		TransGui::DrawTransitionStateUI("Title  -> Wait"  , TitleToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Game"  , WaitToGame);
		TransGui::DrawTransitionStateUI("Game   -> Wait"  , GameToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Result", WaitToResult);
		TransGui::DrawTransitionStateUI("Result -> Title" , ResultToTitle);
		TransGui::DrawTransitionStateUI("Result -> Game"  , ResultToGame);
	}
	ImGui::End();
}
