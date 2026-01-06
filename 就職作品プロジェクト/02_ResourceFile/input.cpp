#include "input.h"
#include "Application.h"
#include "Renderer.h"
#ifdef _DEBUG
#include "DebugUI.h"
#include "imgui.h"
#endif

POINT Input::m_MousePos = {};
POINT Input::m_MouseDelta = {};
bool Input::m_MouseButtons[5] = {};
bool Input::m_MouseButtonsOld[5] = {};
int Input::m_MouseWheel = 0;

BYTE Input::keyState[256] = {};
BYTE Input::keyState_old[256] = {};
XINPUT_STATE Input::controllerState = {};
XINPUT_STATE Input::controllerState_old = {};
int Input::VibrationTime = 0;

static bool IsDownAsync(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

Input::Input()
{
}

Input::~Input()
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

void Input::Update(HWND hWnd)
{
	for (int i = 0; i < 256; i++) { keyState_old[i] = keyState[i]; }
	controllerState_old = controllerState;							

	for (int i = 0; i < 5; ++i) {
		m_MouseButtonsOld[i] = m_MouseButtons[i];
	}

	BOOL hr = GetKeyboardState(keyState);

	POINT prevPos = m_MousePos;
	POINT currentPos{};

	bool usedImGuiPos = false;
	bool ignoreMouseInput = false;

#ifdef _DEBUG
	ImVec2 imageMin{};
	ImVec2 imageMax{};
	if (DebugUI::GetGameViewRect(imageMin, imageMax))
	{
		const ImVec2 gameSize = DebugUI::GetGameRenderSize();
		const ImVec2 mousePos = ImGui::GetMousePos();
		const ImVec2 displaySize(imageMax.x - imageMin.x, imageMax.y - imageMin.y);
		const bool inBounds = mousePos.x >= imageMin.x && mousePos.y >= imageMin.y &&
			mousePos.x <= imageMax.x && mousePos.y <= imageMax.y;
		if (inBounds && displaySize.x > 0.0f && displaySize.y > 0.0f &&
			gameSize.x > 0.0f && gameSize.y > 0.0f)
		{
			const float localX = mousePos.x - imageMin.x;
			const float localY = mousePos.y - imageMin.y;
			const float uvx = localX / displaySize.x;
			const float uvy = localY / displaySize.y;
			const float gameX = uvx * gameSize.x;
			const float gameY = uvy * gameSize.y;

			m_MousePos.x = static_cast<LONG>(gameX - gameSize.x * 0.5f);
			m_MousePos.y = static_cast<LONG>(-gameY + gameSize.y * 0.5f);
			m_MouseDelta.x = m_MousePos.x - prevPos.x;
			m_MouseDelta.y = m_MousePos.y - prevPos.y;
			usedImGuiPos = true;
		}
		else if (!inBounds)
		{
			m_MousePos = prevPos;
			m_MouseDelta.x = 0;
			m_MouseDelta.y = 0;
			usedImGuiPos = true;
			ignoreMouseInput = true;
		}
	}
#endif

	if (!usedImGuiPos) {
		if (GetCursorPos(&currentPos)) {
			if (hWnd != nullptr) {
				ScreenToClient(hWnd, &currentPos);
			}
			m_MousePos = currentPos;

			m_MousePos.x -= static_cast<LONG>(Application::GetWidth() * 0.5f);
			m_MousePos.y = -m_MousePos.y + static_cast<LONG>(Application::GetHeight() * 0.5f);

			m_MouseDelta.x = m_MousePos.x - prevPos.x;
			m_MouseDelta.y = m_MousePos.y - prevPos.y;
		}
		else {
			m_MouseDelta.x = 0;
			m_MouseDelta.y = 0;
		}
	}

	m_MouseButtons[vkLEFT]	   = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	m_MouseButtons[vkRIGHT]    = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
	m_MouseButtons[vkMIDDLE]   = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
	m_MouseButtons[vkXBUTTON1] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
	m_MouseButtons[vkXBUTTON2] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
	if (ignoreMouseInput)
	{
		for (int i = 0; i < 5; ++i)
		{
			m_MouseButtons[i] = false;
			m_MouseButtonsOld[i] = false;
		}
	}
	m_MouseWheel = 0;

	XInputGetState(0, &controllerState);

	if (VibrationTime > 0) {
		VibrationTime--;
		if (VibrationTime == 0) {
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);
		}
	}

}

bool Input::GetMousePress(MOUSE_BUTTON mouseButton)
{
	return m_MouseButtons[mouseButton];
}

bool Input::GetMouseTrriger(MOUSE_BUTTON mouseButton)
{
	return m_MouseButtons[mouseButton] && !m_MouseButtonsOld[mouseButton];
}

bool Input::GetMouseRelease(MOUSE_BUTTON mouseButton)
{
	return !m_MouseButtons[mouseButton] && m_MouseButtonsOld[mouseButton];
}

DirectX::SimpleMath::Vector2 Input::GetMousePos()
{
    return DirectX::SimpleMath::Vector2(static_cast<float>(m_MousePos.x), static_cast<float>(m_MousePos.y));;
}

DirectX::SimpleMath::Vector2 Input::GetMouseDelta()
{
    return DirectX::SimpleMath::Vector2(static_cast<float>(m_MouseDelta.x), static_cast<float>(m_MouseDelta.y));;
}

int Input::GetWheel()
{
	return m_MouseWheel;
}


//キー入力
bool Input::GetKeyPress(int key) //プレス
{
	return keyState[key] & 0x80;
}
bool Input::GetKeyTrigger(int key) //トリガー
{
	return (keyState[key] & 0x80) && !(keyState_old[key] & 0x80);
}
bool Input::GetKeyRelease(int key) //リリース
{
	return !(keyState[key] & 0x80) && (keyState_old[key] & 0x80);
}

//左アナログスティック
DirectX::SimpleMath::Vector2 Input::GetLeftAnalogStick(void)
{
	SHORT x = controllerState.Gamepad.sThumbLX; // -32768～32767
	SHORT y = controllerState.Gamepad.sThumbLY; // -32768～32767

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f; //-1～1
	res.y = y / 32767.0f; //-1～1
	return res;
}

//右アナログスティック
DirectX::SimpleMath::Vector2 Input::GetRightAnalogStick(void)
{
	SHORT x = controllerState.Gamepad.sThumbRX; // -32768～32767
	SHORT y = controllerState.Gamepad.sThumbRY; // -32768～32767

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f; //-1～1
	res.y = y / 32767.0f; //-1～1
	return res;
}

//左トリガー
float Input::GetLeftTrigger(void)
{
	BYTE t = controllerState.Gamepad.bLeftTrigger; // 0～255
	return t / 255.0f;
}

//右トリガー
float Input::GetRightTrigger(void)
{
	BYTE t = controllerState.Gamepad.bRightTrigger; // 0～255
	return t / 255.0f;
}

//ボタン入力
bool Input::GetButtonPress(WORD btn) //プレス
{
	return (controllerState.Gamepad.wButtons & btn) != 0;
}
bool Input::GetButtonTrigger(WORD btn) //トリガー
{
	return (controllerState.Gamepad.wButtons & btn) != 0 && (controllerState_old.Gamepad.wButtons & btn) == 0;
}
bool Input::GetButtonRelease(WORD btn) //リリース
{
	return (controllerState.Gamepad.wButtons & btn) == 0 && (controllerState_old.Gamepad.wButtons & btn) != 0;
}


//振動
void Input::SetVibration(int frame, float powor)
{
	// XINPUT_VIBRATION構造体のインスタンスを作成
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	// モーターの強度を設定（0～65535）
	vibration.wLeftMotorSpeed = (WORD)(powor * 65535.0f);
	vibration.wRightMotorSpeed = (WORD)(powor * 65535.0f);
	XInputSetState(0, &vibration);

	//振動継続時間を代入
	VibrationTime = frame;
}
