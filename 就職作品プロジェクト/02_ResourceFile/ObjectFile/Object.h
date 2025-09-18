#pragma once
#include "../Transform.h"
#include "../Shader.h"
#include "../input.h"
#include "../Camera.h"
#include "../Texture.h"

class Object : public Transform
{
protected:
	// 描画の為の情報（見た目に関わる部分）
	Color   m_Color;

	BaseShader* m_VertexShader    = nullptr;
	BaseShader* m_PixelShader     = nullptr;
	BaseShader* m_ComputeShader   = nullptr;
	BaseShader* m_GeometoryShader = nullptr;

	Camera*  m_Camera;
	Texture* m_Texture;
public:

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

	void SetColor(const Color color);
	void SetColor(const float r, const float g, const float b, const float a);
	
	void SetTexture(Texture* setTexture);
};
