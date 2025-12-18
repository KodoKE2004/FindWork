#include "input.h"
#include "Application.h"

BYTE Input::keyState[256] = {};
BYTE Input::keyState_old[256] = {};
XINPUT_STATE Input::controllerState = {};
XINPUT_STATE Input::controllerState_old = {};
uint32_t Input::m_MouseNowMask = 0;
uint32_t Input::m_MouseOldMask = 0;
POINT Input::m_MousePos	   = {};
POINT Input::m_MouseOldPos = {};
POINT Input::m_MouseDelta  = {};
int Input::VibrationTime = 0;

static bool IsDownAsync(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

uint32_t Input::VkToMouseButton(int key)
{
	switch (key)
	{
	case VK_LBUTTON: return 0x0001;
	case VK_RBUTTON: return 0x0002;
	case VK_MBUTTON: return 0x0004;
	case VK_XBUTTON1:return 0x0008;
	case VK_XBUTTON2:return 0x0010;
	default:         return 0x0000;
	}
}

//コンストラクタ
Input::Input()
{
	
}

//デストラクタ
Input::~Input()
{
	//振動を終了させる
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

void Input::Update(HWND hWnd)
{
	//1フレーム前の入力を記録しておく
	for (int i = 0; i < 256; i++) { keyState_old[i] = keyState[i]; }	// キー
    controllerState_old = controllerState;								// コントローラー
    m_MouseOldMask = m_MouseNowMask;									// マウス

	//キー入力を更新
	BOOL hr = GetKeyboardState(keyState);
	POINT mousePos;

    // マウス座標の取得
    GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);
    // 画面中央を基準にする
    mousePos.x -= static_cast<LONG>(Application::GetGameWidth()  * 0.5f);
    mousePos.y  = - mousePos.y - static_cast<LONG>(Application::GetGameHeight() * 0.5f);
    m_MousePos = mousePos;

	// Delta差分取得
	m_MouseDelta.x = m_MousePos.x - m_MouseOldPos.x;
	m_MouseDelta.y = m_MousePos.y - m_MouseOldPos.y;

	// ボタン(ポーリング)
	m_MouseNowMask = 0;
	if(IsDownAsync(VK_LBUTTON )) m_MouseNowMask |= VkToMouseButton(VK_LBUTTON );
	if(IsDownAsync(VK_RBUTTON )) m_MouseNowMask |= VkToMouseButton(VK_RBUTTON );
	if(IsDownAsync(VK_MBUTTON )) m_MouseNowMask |= VkToMouseButton(VK_MBUTTON );
	if(IsDownAsync(VK_XBUTTON1)) m_MouseNowMask |= VkToMouseButton(VK_XBUTTON1);
	if(IsDownAsync(VK_XBUTTON2)) m_MouseNowMask |= VkToMouseButton(VK_XBUTTON2);

	//コントローラー入力を更新(XInput)
	XInputGetState(0, &controllerState);

	//振動継続時間をカウント
	if (VibrationTime > 0) {
		VibrationTime--;
		if (VibrationTime == 0) { //振動継続時間が経った時に振動を止める
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed  = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);
		}
	}




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

bool Input::GetMousePress(int vk)
{
	const uint32_t m = VkToMouseButton(vk);
	return (m_MouseNowMask & m) != 0;
}

bool Input::GetMouseTrigger(int vk)
{
	const uint32_t m = VkToMouseButton(vk);
	return ((m_MouseNowMask & m) != 0) && ((m_MouseOldMask & m) == 0);
}

bool Input::GetMouseRelease(int vk)
{
	const uint32_t m = VkToMouseButton(vk);
	return ((m_MouseNowMask & m) == 0) && ((m_MouseOldMask & m) != 0);
}

POINT Input::GetMousePos()
{
	return m_MousePos;
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
