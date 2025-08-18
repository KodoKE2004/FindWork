#pragma once
#include "Cube.h"
#include "../Renderer.h"
#include	<DirectXMath.h>
#include	<SimpleMath.h>

class Light : public Cube
{

private:
	Material m_LightBuffer; // ライトバッファ
public:
	Light(Camera* cam);
	~Light() = default;

	void Initialize() override;
	void Update() override ;
	void Draw() override ;

	void SetDirection(const DirectX::SimpleMath::Vector4& direction) { m_LightBuffer.LightDirection = DirectX::SimpleMath::Vector4(direction); }
	void SetLightColor(const DirectX::SimpleMath::Color& color) { m_LightBuffer.LightColor = color; }
	void SetAmbient(float intensity) { m_LightBuffer.AmbientIntensity = intensity; }
	void SetDiffuse(float intensity) { m_LightBuffer.DiffuseIntensity = intensity; }
	void SetSpecular(float intensity) { m_LightBuffer.SpecularIntensity = intensity; }
	void SetLightPosition(const DirectX::SimpleMath::Vector3& position) { SetPos(position.x, position.y, position.z); } // ライトの位置を設定

};

