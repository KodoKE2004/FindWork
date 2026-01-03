#pragma once
#include "Camera.h"
#include "input.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

#include <memory>

// タイマー用の構造体
class TimerData
{
public:
	float timer = 0.0f;
	float limit = 0.0f;

	bool IsTimeUp() const {
		return timer >= limit;
	}
};

class Object : public Transform
{
protected:
	std::string m_Name;
	// 描画の為の情報（見た目に関わる部分）
	Camera&  m_Camera;
	Color    m_Color;

	BaseShader* m_VertexShader    = nullptr;
	BaseShader* m_PixelShader     = nullptr;
	BaseShader* m_ComputeShader   = nullptr;
	BaseShader* m_GeometoryShader = nullptr;

	std::shared_ptr<Texture> m_Texture = nullptr;

public:

	Object(Camera& cam);
	virtual ~Object();

	virtual void Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Draw()       = 0;
	virtual void Finalize()   = 0;

	// シェーダーの追加
	void SetShader(std::string hlslName);
	void SetShader(std::string hlslName1,std::string hlslName2 );
	void SetShader(std::string hlslName1,std::string hlslName2 ,std::string hlslName3);
	void SetShader(std::string hlslName1,std::string hlslName2 ,std::string hlslName3,std::string hlslName4);

	void SetGPU();

	void SetName(std::string name);

	void SetTexture(std::shared_ptr<Texture> setTexture);
	std::shared_ptr<Texture> GetTexture() const { 
		return m_Texture; 
	}

	void SetColor(const Color color);
	void SetColor(const float r, const float g, const float b, const float a);
	
	Color GetColor() {
		return m_Color;
	}
	std::string GetName() {
		return m_Name;
	}
	

};
