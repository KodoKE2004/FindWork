#include "input.h"
#include "Application.h"

DirectX::Mouse::State			   Input::m_State;
DirectX::Mouse::ButtonStateTracker Input::m_Tracker;
DirectX::SimpleMath::Vector2	   Input::m_MousePos;
DirectX::SimpleMath::Vector2	   Input::m_MouseDelta;

BYTE Input::keyState[256] = {};
BYTE Input::keyState_old[256] = {};
XINPUT_STATE Input::controllerState = {};
XINPUT_STATE Input::controllerState_old = {};
int Input::VibrationTime = 0;

static bool IsDownAsync(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
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
    auto& mouse = DirectX::Mouse::Get();

	//キー入力を更新
	BOOL hr = GetKeyboardState(keyState);

    // マウス座標の取得

	// 現在の状態を取得
    m_State = mouse.GetState();
    m_Tracker.Update(m_State);

    // float型に変換
	m_MousePos = DirectX::SimpleMath::Vector2(
		static_cast<float>(m_State.x),
		static_cast<float>(m_State.y)
	);

	m_MouseDelta = DirectX::SimpleMath::Vector2(
		static_cast<float>(m_State.x),
		static_cast<float>(m_State.y)
	);

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

bool Input::GetMousePress(MOUSE_BUTTON mouseButton)
{
	switch (mouseButton)
	{
	case MOUSE_BUTTON::vkLEFT	 :	return m_State.leftButton	; break;
	case MOUSE_BUTTON::vkRIGHT	 :	return m_State.rightButton	; break;
	case MOUSE_BUTTON::vkMIDDLE	 :	return m_State.middleButton	; break;
	case MOUSE_BUTTON::vkXBUTTON1:	return m_State.xButton1		; break;
	case MOUSE_BUTTON::vkXBUTTON2:	return m_State.xButton2		; break;
	}
}

bool Input::GetMouseTrriger(MOUSE_BUTTON mouseButton)
{
	switch (mouseButton)
	{
	case MOUSE_BUTTON::vkLEFT	 :	return m_State.leftButton	== DirectX::Mouse::ButtonStateTracker::PRESSED; break;
	case MOUSE_BUTTON::vkRIGHT	 :	return m_State.rightButton	== DirectX::Mouse::ButtonStateTracker::PRESSED; break;
	case MOUSE_BUTTON::vkMIDDLE	 :	return m_State.middleButton	== DirectX::Mouse::ButtonStateTracker::PRESSED; break;
	case MOUSE_BUTTON::vkXBUTTON1:	return m_State.xButton1		== DirectX::Mouse::ButtonStateTracker::PRESSED; break;
	case MOUSE_BUTTON::vkXBUTTON2:	return m_State.xButton2		== DirectX::Mouse::ButtonStateTracker::PRESSED; break;
	}
}

bool Input::GetMouseRelease(MOUSE_BUTTON mouseButton)
{
	switch (mouseButton)
	{
	case MOUSE_BUTTON::vkLEFT	 :	return m_State.leftButton	== DirectX::Mouse::ButtonStateTracker::RELEASED; break;
	case MOUSE_BUTTON::vkRIGHT	 :	return m_State.rightButton	== DirectX::Mouse::ButtonStateTracker::RELEASED; break;
	case MOUSE_BUTTON::vkMIDDLE	 :	return m_State.middleButton	== DirectX::Mouse::ButtonStateTracker::RELEASED; break;
	case MOUSE_BUTTON::vkXBUTTON1:	return m_State.xButton1		== DirectX::Mouse::ButtonStateTracker::RELEASED; break;
	case MOUSE_BUTTON::vkXBUTTON2:	return m_State.xButton2		== DirectX::Mouse::ButtonStateTracker::RELEASED; break;
	}
}

DirectX::SimpleMath::Vector2 Input::GetMousePos()
{
    return m_MousePos;
}

DirectX::SimpleMath::Vector2 Input::GetMouseDelta()
{
    return m_MouseDelta;
}

int Input::GetWheel()
{
	return m_State.scrollWheelValue;
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
