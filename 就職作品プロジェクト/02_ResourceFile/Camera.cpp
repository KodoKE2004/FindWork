#include "Camera.h"
#include "Renderer.h"
#include "Application.h"
#include "Input.h"
#include "Game.h"
#include "DebugUI.h"

using namespace DirectX::SimpleMath;


//
// 
// ======
//初期化処理
//-------------------------------======
void Camera::Initialize()
{
	m_Position  = Vector3(0.0f, 20.0f, 250.0f);
	m_Rotation  = Vector3(0.0f,  0.0f,   0.0f);
	m_Up        = Vector3(0.0f,  1.0f,   0.0f);

#ifdef _DEBUG
	DebugUI::RedistDebugFunction([this]() {
		DebugCameraUI();
	});
	DebugUI::RedistDebugFunction([this]() {
		DebugCameraParamUI();
	});
#endif //DEBUG

}


//-------------------------------======
//更新処理
//-------------------------------======
void Camera::Update()
{
	SetCamera(m_Mode);
}

//-------------------------------======
//終了処理
//-------------------------------======
void Camera::Finalilze()
{

}

void Camera::SetCamera(CAMERA_MODE mode)
{
	m_Mode = mode;
	
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
	// ビルボードのような
	// 平面に張り付かせる
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

#ifdef _DEBUG
void Camera::DebugCameraUI()
{
	ImGuiIO& io = ImGui::GetIO();

	// マウスドラッグでPan（右クリック押しながら）
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
		ImVec2 delta = io.MouseDelta;
		this->Pan(delta.x * m_PanPitch, delta.y * - m_PanYaw);  // スケーリングは調整して
	}

	// マウススクロール（Ctrlなし → Dolly、Ctrlあり → Zoom）
	if (io.MouseWheel != 0.0f) {
		if (io.KeyCtrl) {
			this->Zoom(io.MouseWheel * - m_Zoom);
		}
		else {
			this->Dolly(io.MouseWheel * m_DollySpeed);
		}
	}

	// ミドルボタン（スクロールボタン）ドラッグでTrack
	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		ImVec2 delta = io.MouseDelta;
		this->Track(delta.x * m_TrackX, delta.y * m_TrackY);  // X = 横, Y = 上下
	}

}

void Camera::DebugCameraParamUI()
{
	if (ImGui::Begin("Camera Parameters"))
	{
		// 位置（XYZ）
		ImGui::Text("Position");
		ImGui::DragFloat3("##Pos", &m_Position.x, 0.1f);

		// 回転（XYZ）※度数で表示
		ImGui::Text("Rotation (Degrees)");
		ImGui::DragFloat3("##Rot", &m_Rotation.x, 0.5f);

		// FOV（視野角）
		ImGui::SliderFloat("FOV", &m_Fov, 10.0f, 90.0f);

		// ニア・ファークリップ
		ImGui::DragFloat("Near Plane", &m_NearPlane, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Far Plane", &m_FarPlane, 1.0f, 10.0f, 10000.0f);

		// スピード（オプション）
		ImGui::DragFloat("Speed", &m_Speed, 0.1f, 0.1f, 100.0f);

		// その他パラメータ（DollyやZoom調整用など）
		ImGui::Separator();
		ImGui::Text("Adjustment Values");
		ImGui::DragFloat("Dolly Speed", &m_DollySpeed, 0.1f);
		ImGui::DragFloat("Track X", &m_TrackX, 0.1f);
		ImGui::DragFloat("Track Y", &m_TrackY, 0.1f);
		ImGui::DragFloat("Pan Yaw", &m_PanYaw, 0.001f);
		ImGui::DragFloat("Pan Pitch", &m_PanPitch, 0.001f);
		ImGui::DragFloat("Roll", &m_Roll, 0.1f);
		ImGui::DragFloat("Zoom", &m_Zoom, 0.1f);

		// カメラの再セット（リアルタイム反映用）
		if (ImGui::Button("Apply Parameters"))
		{
			SetCamera(m_Mode); // 現在のモードで再設定（ビュー・プロジェクション更新）
		}
	}
	ImGui::End();
}
#endif

Camera::ScopedMode::ScopedMode(Camera* camera, CAMERA_MODE mode) : m_Camera(camera)
{
	if(m_Camera == nullptr) return;

	m_PreviousMode = m_Camera->GetMode();
	if (m_PreviousMode != mode)
	{
		m_Camera->SetCamera(mode);
		m_ShouldRestore = true;
	}
}

Camera::ScopedMode::~ScopedMode()
{
	Restore();
}

void Camera::ScopedMode::Restore()
{
	if (m_Camera && m_ShouldRestore)
	{
		m_Camera->SetCamera(m_PreviousMode);
		m_ShouldRestore = false;
	}
}
