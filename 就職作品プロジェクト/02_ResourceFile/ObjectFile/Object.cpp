#include "Object.h"
#include "../Game.h"

Object::Object(Camera* cam) : m_Camera(cam)
{

}

Object::~Object()
{
}

void Object::SetShader(std::string hlslName)
{
	m_Shaders.push_back(GAME_MANAGER_SHADER->GetShader(hlslName));
}

void Object::SetShader(std::string vsName, std::string psName)
{
	m_Shaders.clear(); // 既存のシェーダーをクリア
	m_Shaders.push_back(GAME_MANAGER_SHADER->GetShader(vsName));
	m_Shaders.push_back(GAME_MANAGER_SHADER->GetShader(psName));
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
