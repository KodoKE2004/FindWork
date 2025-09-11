#pragma once
#include "Object.h"

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"

class Sphere : public Object
{
protected:
	VertexBuffer<VERTEX_3D> m_VertexBuffer;
	IndexBuffer				m_IndexBuffer;
	Texture*				m_Texture;
public:
	unsigned int m_IndexCount = 0;
	int			 m_Sline;
	int			 m_Stack;
	float		 m_Radius;

public:
	Sphere(Camera* cam);

	void Initialize() override;
	void Update()	  override;
	void Draw()		  override;
	void Finalize()	  override;

};

