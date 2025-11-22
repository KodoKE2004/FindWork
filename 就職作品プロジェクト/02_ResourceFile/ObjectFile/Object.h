#pragma once
#include "Transform.h"
#include "Shader.h"
#include "input.h"
#include "Camera.h"
#include "Texture.h"

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

// 当たり判定の結果を受け取るクラス
class isHitResult
{
private: 
	bool m_Old = false;
	bool m_Now = false;
public:
	
	bool isTriggered() {
		return (!m_Old && m_Now);
    }
	bool isReleased() {
		return (m_Old && !m_Now);
    }
	bool isHold() {
		return m_Now;
    }

    void SetHitResult(const bool hit) 
	{
		m_Old = m_Now;
		m_Now = hit;
    }

	void Update()
	{
		m_Old = m_Now;
    }
};


class Object : public Transform
{
protected:
	std::string m_Name;
	// 描画の為の情報（見た目に関わる部分）
	Camera*  m_Camera;
	Color   m_Color;

	BaseShader* m_VertexShader    = nullptr;
	BaseShader* m_PixelShader     = nullptr;
	BaseShader* m_ComputeShader   = nullptr;
	BaseShader* m_GeometoryShader = nullptr;

	std::shared_ptr<Texture> m_Texture = nullptr;

public:
    isHitResult m_HitResult;

	Object(Camera* cam);
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

	void SetColor(const Color color);
	void SetColor(const float r, const float g, const float b, const float a);
	
	Color GetColor() {
		return m_Color;
	}
	std::string GetName() {
		return m_Name;
	}
	

};
