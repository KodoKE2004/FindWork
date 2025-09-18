#include "Object.h"
#include "../Game.h"
#include "../ShaderManager.h"

Object::Object(Camera* cam) : m_Camera(cam)
{

}

Object::~Object()
{
}

void Object::SetShader(std::string hlslName)
{
	if (hlslName.rfind("VS_", 0) == 0) { m_VertexShader    = GAME_MANAGER_SHADER->GetShader(hlslName);}
	if (hlslName.rfind("PS_", 0) == 0) { m_PixelShader     = GAME_MANAGER_SHADER->GetShader(hlslName);}
	if (hlslName.rfind("GS_", 0) == 0) { m_GeometoryShader = GAME_MANAGER_SHADER->GetShader(hlslName);}
	if (hlslName.rfind("CS_", 0) == 0) { m_ComputeShader   = GAME_MANAGER_SHADER->GetShader(hlslName);}
}

void Object::SetShader(std::string hlslName1, std::string hlslName2)
{
	SetShader(hlslName1);
	SetShader(hlslName2);
}

void Object::SetShader(std::string hlslName1, std::string hlslName2, std::string hlslName3)
{
	SetShader(hlslName1);
	SetShader(hlslName2);
	SetShader(hlslName3);
}

void Object::SetShader(std::string hlslName1, std::string hlslName2, std::string hlslName3, std::string hlslName4)
{
	SetShader(hlslName1);
	SetShader(hlslName2);
	SetShader(hlslName3);
	SetShader(hlslName4);
}

void Object::SetGPU()
{
	if(m_VertexShader    != nullptr) { m_VertexShader   ->SetGPU(); }
	if(m_PixelShader     != nullptr) { m_PixelShader    ->SetGPU(); }
	if(m_ComputeShader   != nullptr) { m_ComputeShader  ->SetGPU(); }
	if(m_GeometoryShader != nullptr) { m_GeometoryShader->SetGPU(); }
}

void Object::SetColor(const Color color)
{
	m_Color = color;
}

void Object::SetColor(const float r, const float g, const float b, const float a)
{
	Color color = Color(r, g, b, a);
	m_Color = color;
}

void Object::SetTexture(Texture* setTexture)
{
	m_Texture = setTexture;
}
