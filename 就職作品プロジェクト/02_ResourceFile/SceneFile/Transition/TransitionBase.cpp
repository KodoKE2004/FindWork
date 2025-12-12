#include <cassert>
#include <cmath>
#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

#include "TransitionBase.h"
#include "Collider.h"
#ifdef _DEBUG
#include "Application.h"
#include "Renderer.h"
#endif

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

namespace
{
	struct TransitionEntry
	{
		const char* from;
		const char* to;
        SceneTransitionParam* param;
    };

	constexpr TransitionEntry kTransitionEntries[] = 
	{
		{"Title",  "Wait",   &TitleToWait},
		{"Wait",   "Game",   &WaitToGame},
		{"Game",   "Wait",   &GameToWait},
		{"Wait",   "Result", &WaitToResult},
		{"Result", "Title",  &ResultToTitle},
		{"Result", "Game",   &ResultToGame},
    };

	std::string Trim(const std::string& value)
	{
        const auto first = std::find_if_not(value.begin(),
											value.end(),
											[](unsigned char ch) {
												return std::isspace(ch);
											});

		const auto last = std::find_if_not(value.rbegin(),
										   value.rend(),
										   [](unsigned char ch) {
											   return std::isspace(ch);
										    }).base();

		if (first >= last) {
			return {};
		}
        return std::string(first, last);
	}

	bool TryParseTransMode(const std::string& label, TRANS_MODE& mode)
	{
		for (int i = 0; i < TransGui::kTransModeCount; ++i)
		{
			if (label == TransGui::kTransModeLabels[i])
			{
				mode = static_cast<TRANS_MODE>(i);
				return true;
			}
		}
		return false;
	}

	bool TryParseEasingType(const std::string& label, EASING_TYPE& easing)
	{
		for (int i = 0; i < TransGui::kEaseingTypeCount; ++i)
		{
			if (label == TransGui::kEasingLabels[i])
			{
				easing = static_cast<EASING_TYPE>(i);
				return true;
			}
		}
		return false;
	}
}

bool SaveTransitionSettingsToCsv(const std::string& filePath, std::string& errorMessage)
{
	std::filesystem::path path(filePath);
	if (!path.parent_path().empty())
	{
		std::error_code ec{};
		std::filesystem::create_directories(path.parent_path(), ec);
		if (ec)
		{
			errorMessage = "Failed to create directories: " + ec.message();
			return false;
		}
	}

	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);	
	if (!ofs.is_open())
	{
		errorMessage = "Failed to open file for writing: " + filePath;
		return false;
	}

	const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
	ofs.write(reinterpret_cast<const char*>(bom), 3);

	ofs.imbue(std::locale::classic());
	ofs << "from,to,mode,duration,easing\r\n";
	for (const auto& entry : kTransitionEntries)
	{
		const auto& param = *entry.param;
		ofs << entry.from << ',' << entry.to << ','
			<< TransGui::kTransModeLabels[param.ModeAsIndex()] << ','
			<< param.duration << ','
			<< TransGui::kEasingLabels[param.EasingAsIndex()] << '\r\n';
	}

	return static_cast<bool>(ofs);
}

bool LoadTransitionSettingsFromCsv(const std::string& filePath, std::string& errorMessage)
{
	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		errorMessage = "[[失敗]] ファイルを開けませんでした。" + filePath;
		return false;
	}

	std::unordered_map<std::string, SceneTransitionParam*> mapping;
	for (const auto& entry : kTransitionEntries)
	{
		mapping.emplace(std::string(entry.from) + "->" + entry.to, entry.param);
	}

	std::string line;
	bool hasAny = false;
	while (std::getline(ifs, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::stringstream ss(line);
		std::string from;
		std::string to;
		std::string modeLabel;
		std::string durationStr;
		std::string easingLabel;

		if (!std::getline(ss, from, ',')) continue;
		if (!std::getline(ss, to, ',')) continue;
		if (!std::getline(ss, modeLabel, ',')) continue;
		if (!std::getline(ss, durationStr, ',')) continue;
		if (!std::getline(ss, easingLabel)) continue;

		from = Trim(from);
		to   = Trim(to);
		modeLabel   = Trim(modeLabel);
		durationStr = Trim(durationStr);
		easingLabel = Trim(easingLabel);

		if (from == "from" && to == "to")
		{
			continue;
		}

		auto it = mapping.find(from + "->" + to);
		if (it == mapping.end())
		{
			errorMessage = "[[失敗]] 未定義のシーン遷移です : " + from + " -> " + to;
			return false;
		}

		TRANS_MODE mode{};
		if (!TryParseTransMode(modeLabel, mode))
		{
			errorMessage = "[[失敗]] 不明な遷移モードです : " + modeLabel;
			return false;
		}

		EASING_TYPE easing{};
		if (!TryParseEasingType(easingLabel, easing))
		{
			errorMessage = "[[失敗]] 不明なイージングタイプです : " + easingLabel;
			return false;
		}

		float duration = 0.0f;
		try
		{
			duration = std::stof(durationStr);
		}
		catch (...)
		{
			errorMessage = "[[失敗]] 遷移時間が不正です : " + durationStr;
			return false;
		}

		SceneTransitionParam* param = it->second;
		param->mode = mode;
		param->duration = duration;
		param->easing = easing;
		hasAny = true;
	}

	if (!hasAny)
	{
		errorMessage = "[[失敗]] 有効なシーン遷移設定が見つかりませんでした : " + filePath;
		return false;
	}

	return true;
}

#ifdef _DEBUG
NVector3 TransitionBase::ConvertToDebugScreenPosition(const NVector3& position) const
{
	const float debugWidth = Renderer::GetScreenWidth();
	const float debugHeight = Renderer::GetScreenHeight();
	const float gameWidth = static_cast<float>(Application::GetGameWidth());
	const float gameHeight = static_cast<float>(Application::GetGameHeight());

	const float offsetX = (debugWidth - gameWidth) * 0.5f;
	const float offsetY = (debugHeight - gameHeight) * 0.5f;

	return NVector3(position.x + offsetX, position.y - offsetY, position.z);
}
#endif

TransitionBase::TransitionBase(Camera& cam) : Object(cam)
{
    m_Texture = std::make_shared<Texture>();
	m_TransMode = TRANS_MODE::NUM;
    m_Phase = TRANS_PHASE::NONE;
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

void TransitionBase::ApplyPhaseSetting(TRANS_PHASE phase)
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
		static char csvPath[260] = "TransitionData.csv";
		static std::string csvStatus;

		ImGui::InputText("CSV Path", csvPath, IM_ARRAYSIZE(csvPath));
		if (ImGui::Button("Load CSV"))
		{
			std::string error;
			if (LoadTransitionSettingsFromCsv(csvPath, error))
			{
				csvStatus = std::string("true LoadCSV : ") + csvPath;
			}
			else
			{
				csvStatus = std::string("false LoadCSV : ") + error;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Save CSV"))
		{
			std::string error;
			if (SaveTransitionSettingsToCsv(csvPath, error))
			{
				csvStatus = std::string("ture SaveCSV : ") + csvPath;
			}
			else
			{
				csvStatus = std::string("false SaveCSV : ") + error;
			}
		}

		if (!csvStatus.empty())
		{
			ImGui::TextWrapped("%s", csvStatus.c_str());
		}

		ImGui::Separator();

		TransGui::DrawTransitionStateUI("Title  -> Wait",   TitleToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Game",   WaitToGame);
		TransGui::DrawTransitionStateUI("Game   -> Wait",   GameToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Result", WaitToResult);
		TransGui::DrawTransitionStateUI("Result -> Title",  ResultToTitle);
		TransGui::DrawTransitionStateUI("Result -> Game",   ResultToGame);
	}
	ImGui::End();
}
