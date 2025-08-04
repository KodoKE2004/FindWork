#include "Object.h"

Object::Object(Camera* cam) : m_Camera(cam)
{

}

Object::~Object()
{
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
