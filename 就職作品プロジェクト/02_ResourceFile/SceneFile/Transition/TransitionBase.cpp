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
#include "Application.h"
#include "Renderer.h"
#include "Game.h"

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
	ofs << "from,to,mode,duration,easing\n";
	for (const auto& entry : kTransitionEntries)
	{
		const auto& param = *entry.param;
		ofs << entry.from << ',' << entry.to << ','
			<< TransGui::kTransModeLabels[param.ModeAsIndex()] << ','
			<< param.duration << ','
			<< TransGui::kEasingLabels[param.EasingAsIndex()] << '\n';
	}

	return static_cast<bool>(ofs);
}


bool LoadTransitionSettingsFromCsv(const std::string& filePath, std::string& errorMessage)
{
	std::ifstream ifs(filePath, std::ios::binary);
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
	std::size_t lineNumber = 0;
	bool hasAny = false;
	while (std::getline(ifs, line))
	{
		++lineNumber;
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}
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

		auto removeBom = [](std::string& value)
			{
				const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
				if (value.size() >= 3 && static_cast<unsigned char>(value[0]) == bom[0]
					&& static_cast<unsigned char>(value[1]) == bom[1]
					&& static_cast<unsigned char>(value[2]) == bom[2])
				{
					value.erase(0, 3);
				}
			};

		auto removeTrailingCr = [](std::string& value)
			{
				if (!value.empty() && value.back() == '\r')
				{
					value.pop_back();
				}
			};

		from = Trim(from);
		to = Trim(to);
		modeLabel = Trim(modeLabel);
		durationStr = Trim(durationStr);
		easingLabel = Trim(easingLabel);

		removeBom(from);
		removeTrailingCr(from);
		removeTrailingCr(to);
		removeTrailingCr(modeLabel);
		removeTrailingCr(durationStr);
		removeTrailingCr(easingLabel);

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

TransitionBase::TransitionBase(Camera& cam) : Object(cam)
{
    m_Texture = std::make_shared<Texture>();
	m_TransMode = TRANS_MODE::NUM;
    m_Phase = TRANS_PHASE::NONE;
}

void TransitionBase::SetPipeline()
{
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
	if (!devicecontext) {
		return;
	}

	// Sprite/FullscreenQuad用: 毎回確実にパイプラインを設定する
	SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Renderer::BindDefaultSamplers();
	Renderer::BindDefaultConstantBuffers();
}

void TransitionBase::DrawDebugFullscreenSolid()
{
	// 呼ばれているが描けていない場合の切り分け: 単色フルスクリーン描画
	static std::shared_ptr<Texture> s_DebugTexture;
	if (!s_DebugTexture) {
		TextureManager* textureMgr = Game::GetInstance();
		s_DebugTexture = textureMgr->GetTexture("Plane.png");
	}

	SetPipeline();
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
	if (!devicecontext) {
		return;
	}
	Renderer::SetDepthEnable(false);
	Renderer::SetBlendState(BS_ALPHABLEND);

	const auto prevColor = m_Color;
	const auto prevTexture = m_Texture;

	if (s_DebugTexture) {
		m_Texture = s_DebugTexture;
	}
	SetColor(m_DebugSolidColor);

	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
	Matrix world = s * r * t;
	Renderer::SetWorldMatrix(&world);

	if (m_Texture) {
		m_Texture->SetGPU();
	}
	if (m_Material) {
		m_Material->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
		m_Material->Update();
		m_Material->SetGPU();
	}

	float u = (m_NumU - 1.0f) / m_SplitX;
	float v = (m_NumV - 1.0f) / m_SplitY;
	float uw = 1.0f / m_SplitX;
	float vh = 1.0f / m_SplitY;
	Renderer::SetUV(u, v, uw, vh);

	Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
	devicecontext->DrawIndexed(4, 0, 0);

	m_Texture = prevTexture;
	SetColor(prevColor);
	Renderer::SetDepthEnable(true);
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

void TransitionBase::ApplyPhaseSetting(TRANS_PHASE phase)
{
	const auto& param = m_transParam;
	m_Duration = max(param.duration, 0.0001f);
	m_Phase = phase;
}


void DrawInGameStateGUI()
{
	if (ImGui::Begin("InGame State Settings"))
	{
        // BGM関連設定
		ImGui::Separator();
		float bpm = Game::GetBgmBpm();
		if (ImGui::DragFloat("Bpm", &bpm, 0.1f, 30.0f, 300.0f, "%.1f"))
		{
			Game::SetBgmBpm(bpm);
		}

		ImGui::Separator();
		ImGui::TextUnformatted("Difficulty/Speed Settings");
		int difficultyStageInterval = Game::GetDifficultyStageInterval();
		if (ImGui::DragInt("Difficulty Stage Interval", &difficultyStageInterval, 1.0f, 1, 100))
		{
			Game::SetDifficultyStageInterval(difficultyStageInterval);
		}
		float baseBpmIncreasePerDifficulty = Game::GetBaseBpmIncreasePerDifficulty();
		if (ImGui::DragFloat("Base BPM Increase / Difficulty", &baseBpmIncreasePerDifficulty, 0.1f, 0.0f, 100.0f, "%.1f"))
		{
			Game::SetBaseBpmIncreasePerDifficulty(baseBpmIncreasePerDifficulty);
		}
		int speedUpStageInterval = Game::GetSpeedUpStageInterval();
		if (ImGui::DragInt("Speed Up Stage Interval", &speedUpStageInterval, 1.0f, 1, 100))
		{
			Game::SetSpeedUpStageInterval(speedUpStageInterval);
		}
		float speedUpBpmIncrease = Game::GetSpeedUpBpmIncrease();
		if (ImGui::DragFloat("Speed Up BPM Increase", &speedUpBpmIncrease, 0.1f, 0.0f, 200.0f, "%.1f"))
		{
			Game::SetSpeedUpBpmIncrease(speedUpBpmIncrease);
		}

        // Transition Settings
        //------------------------------
        //			CSV入出力
        //------------------------------
		static char csvPath[260] = "AssetFile/Csv/TransitionData.csv";
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

		TransGui::DrawTransitionStateUI("Title  -> Wait",   TitleToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Game",   WaitToGame);
		TransGui::DrawTransitionStateUI("Game   -> Wait",   GameToWait);
		TransGui::DrawTransitionStateUI("Wait   -> Result", WaitToResult);
		TransGui::DrawTransitionStateUI("Result -> Title",  ResultToTitle);
		TransGui::DrawTransitionStateUI("Result -> Game",   ResultToGame);
	}
	ImGui::End();
}
