#include "Camera.h"
#include "Renderer.h"
#include "Application.h"
#include "Input.h"
#include "Game.h"
#include "DebugUI.h"

using namespace DirectX::SimpleMath;


//=======================================
//初期化処理
//=======================================
void Camera::Initialize()
{
	m_Position  = Vector3(0.0f, 20.0f, 250.0f);
	m_Rotation  = Vector3(0.0f,-20.0f,   0.0f);
	m_Up        = Vector3(0.0f,  1.0f,   0.0f);

#ifdef _DEBUG
	DebugUI::RedistDebugFunction([this]() {
	DebugCameraUI();
	});
#endif //DEBUG

}


//=======================================
//更新処理
//=======================================
void Camera::Update()
{
	// 操作用入力例（必要に応じて修正可）
	if (Input::GetKeyPress(VK_W)) Dolly(1.0f);
	if (Input::GetKeyPress(VK_S)) Dolly(-1.0f);
	if (Input::GetKeyPress(VK_A)) Track(-1.0f, 0);
	if (Input::GetKeyPress(VK_D)) Track(1.0f, 0);
	if (Input::GetKeyPress(VK_Q)) Track(0, 1.0f);
	if (Input::GetKeyPress(VK_E)) Track(0, -1.0f);

	if (Input::GetKeyPress(VK_LEFT))  Pan(-1.0f, 0);
	if (Input::GetKeyPress(VK_RIGHT)) Pan(1.0f, 0);
	if (Input::GetKeyPress(VK_UP))    Pan(0, 1.0f);
	if (Input::GetKeyPress(VK_DOWN))  Pan(0, -1.0f);

	if (Input::GetKeyPress(VK_Z)) Zoom(-1.0f);
	if (Input::GetKeyPress(VK_X)) Zoom(1.0f);

	if (Input::GetKeyPress(VK_R)) Roll(1.0f);
	if (Input::GetKeyPress(VK_F)) Roll(-1.0f);

}

//=======================================
//終了処理
//=======================================
void Camera::Finalilze()
{

}

void Camera::SetCamera(CAMERA_MODE mode)
{

	//3D
	if (mode == CAMERA_3D) {
		m_Eye      = m_Position;									// カメラの位置
		m_eularRad = Vector3(XMConvertToRadians(m_Rotation.x),
							 XMConvertToRadians(m_Rotation.y),
							 XMConvertToRadians(m_Rotation.z));
		m_RotateQ  = Quaternion::CreateFromYawPitchRoll(m_eularRad.y, m_eularRad.x, m_eularRad.z);
		
		m_Forward = Vector3::Transform(Vector3::Forward, m_RotateQ); // Z-方向を回転
		m_Up      = Vector3::Transform(Vector3::Up     , m_RotateQ); // Y-方向を回転

		m_LookAt = m_Eye + m_Forward;	// 注視点を計算

		// ビュー変換後列作成
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Eye, m_LookAt, m_Up);
		Renderer::SetViewMatrix(&m_ViewMatrix);

		//プロジェクション行列の生成
		float fieldOfView = DirectX::XMConvertToRadians(m_Fov);    // 視野角

		float aspectRatio = static_cast<float>(Application::GetWidth()) / static_cast<float>(Application::GetHeight());	// アスペクト比	
		float nearPlane   = 1.0f;         // ニアクリップ
		float farPlane    = 1000.0f;      // ファークリップ

		//プロジェクション行列の生成
		m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, m_NearPlane, m_FarPlane);
		Renderer::SetProjectionMatrix(&m_ProjectionMatrix);
	}

	//2D
	else if (mode == CAMERA_2D)
	{
		//ビュー変換行列
		Vector3 pos = { 0.0f,  0.0f,-10.0f };
		Vector3 tgt = { 0.0f,  0.0f,  1.0f };
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(pos, tgt, up);
		Renderer::SetViewMatrix(&m_ViewMatrix);

		//プロジェクション行列の生成
		float nearPlane = 1.0f;
		float farPlane = 1000.0f;

		m_ProjectionMatrix = DirectX::XMMatrixOrthographicLH(
			static_cast<float>(Application::GetWidth()),
			static_cast<float>(Application::GetHeight()),
			nearPlane, farPlane);

		m_ProjectionMatrix = DirectX::XMMatrixTranspose(m_ProjectionMatrix);

		Renderer::SetProjectionMatrix(&m_ProjectionMatrix);
	}
}

void Camera::Dolly(float distance)
{
	Quaternion rotateQ = Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.z));

	Vector3 forward = Vector3::Transform(Vector3::Forward, rotateQ);
	m_Position += forward * distance;
	
}

void Camera::Track(float rightDist, float upDist)
{
	Quaternion rotateQ = Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.z));

	Vector3 right = Vector3::Transform(Vector3::Right, rotateQ);
	Vector3 up = Vector3::Transform(Vector3::Up, rotateQ);
	m_Position += right * rightDist + up * upDist;
	
}

void Camera::Pan(float yawDelta, float pitchDelta)
{
	m_Rotation.y += yawDelta;
	m_Rotation.x += pitchDelta;
}

void Camera::Zoom(float fovDelta)
{
	m_Fov = std::clamp(m_Fov + fovDelta, 10.0f, 90.0f);
}

void Camera::Roll(float rollDelta)
{
	m_Rotation.z += rollDelta;
}

void Camera::DebugCameraUI()
{
	ImGuiIO& io = ImGui::GetIO();

	// マウスドラッグでPan（右クリック押しながら）
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
		ImVec2 delta = io.MouseDelta;
		this->Pan(delta.x * 0.1f, delta.y * - 0.1f);  // スケーリングは調整して
	}

	// マウススクロール（Ctrlなし → Dolly、Ctrlあり → Zoom）
	if (io.MouseWheel != 0.0f) {
		if (io.KeyCtrl) {
			this->Zoom(io.MouseWheel * - 5.0f);
		}
		else {
			this->Dolly(io.MouseWheel * 5.0f);
		}
	}

	// ミドルボタン（スクロールボタン）ドラッグでTrack
	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		ImVec2 delta = io.MouseDelta;
		this->Track(delta.x * 0.1f, delta.y * 0.1f);  // X = 横, Y = 上下
	}

}
