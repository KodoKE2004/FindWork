#pragma once
#include "../Transform.h"
#include "../Shader.h"
#include "../input.h"
#include "../Camera.h"


class Object : public Transform
{
protected:
	// 描画の為の情報（見た目に関わる部分）
	Shader m_Shader; // シェーダー
	Camera* m_Camera;

public:

	Object(Camera* cam);
	virtual ~Object();

	virtual void Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Draw()       = 0;
	virtual void Finalize()   = 0;

};
