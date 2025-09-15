#pragma once

#include "Transform.h"
#include <SimpleMath.h>
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

enum CAMERA_MODE {
	CAMERA_2D, // 2Dモード
	CAMERA_3D  // 3Dモード
};

//-----------------------------------------------------------------------------
//Cameraクラス
//-----------------------------------------------------------------------------
class Camera{
private:
	//=================================
	//		   直感的に扱うメンバ変数
	//=================================
	DirectX::SimpleMath::Vector3 m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);	// カメラの位置
	DirectX::SimpleMath::Vector3 m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);	// カメラの回転

	//=================================
	// 		   内部的に扱うメンバ変数
	//=================================
	DirectX::SimpleMath::Vector3    m_Eye{};							// カメラの位置
	DirectX::SimpleMath::Vector3    m_LookAt{ 0.0f, 0.0f, 0.0f };	// 注視点
	DirectX::SimpleMath::Vector3    m_Up    { 0.0f, 1.0f, 0.0f };	// 上方向ベクトル
	DirectX::SimpleMath::Vector3    m_Forward;						// 前方向ベクトル
	DirectX::SimpleMath::Vector3    m_eularRad;						// 注視点　弧度

	DirectX::SimpleMath::Quaternion m_RotateQ{};						// 上方向ベクトルのクォータニオン
	DirectX::SimpleMath::Matrix	   m_ViewMatrix{};					// ビュー行列
	DirectX::SimpleMath::Matrix	   m_ProjectionMatrix{};			// プロジェクション行列

	CAMERA_MODE m_Mode = CAMERA_3D;

	float m_CameraDirection = 0.0f;

	float m_Fov       = 45.0f;		// 視野角
	float m_NearPlane = 0.1f;		// ニアクリップ
	float m_FarPlane  = 10000.0f;	// ファークリップ
	float m_Speed     = 1.0f;		// カメラの移動速度

	float m_DollySpeed = 20.0f;
	float m_TrackX = 0.5f;
	float m_TrackY = 0.5f;
	float m_PanYaw   = 0.05f;
	float m_PanPitch = 0.05f;
	float m_Roll = 1.0f;
	float m_Zoom = 1.0f;
public:

	//===========================================
	//				メインループ処理
	//===========================================
	void Initialize();
	void Update();
	void Finalilze();

	//===========================================
	//				  カメラ設定
	//============================================
	void SetCamera(CAMERA_MODE mode);	
	CAMERA_MODE GetMode() const { return m_Mode; }
	Vector3 GetPosition() const { return m_Position; }
	Vector3 GetForward () const { return m_Forward; }

	//===========================================
	//				   操作処理
	//===========================================
	void Dolly(float distance);
	void Track(float rightDist, float upDist);
	void Pan  (float yetDelta , float pitchDelta);
	void Zoom(float fovDelta);
	void Roll(float rollDelta);

#ifdef _DEBUG
	void DebugCameraUI();
	void DebugCameraParamUI();
#endif // _DEBUG

};