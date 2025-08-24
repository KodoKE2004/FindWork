#pragma once
#include "../Transform.h"
#include "../Shader.h"
#include "../input.h"
#include "../Camera.h"


class Object : public Transform
{
protected:
	// 描画の為の情報（見た目に関わる部分）
	Color   m_Color;
	std::vector<BaseShader*> m_Shaders; // シェーダー
	Camera* m_Camera;
public:

	Object(Camera* cam);
	virtual ~Object();

	virtual void Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Draw()       = 0;
	virtual void Finalize()   = 0;

	void ClearShader() { m_Shaders.clear(); }

	// シェーダーの追加
	void SetShader(std::string hlslName);
	void SetShader(std::string vsName,std::string csName);

	void SetColor(const Color color);
	void SetColor(const float r, const float g, const float b, const float a);
	

};
