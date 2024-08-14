#include "pch.h"
#include "Object.h"
#include "Texture.h"
 

Object::Object()
{
	
	m_pTexture = nullptr;
	m_bActivated = true;
}

Object::~Object()
{
	
}

void Object::LateUpdate()
{
	
}

void Object::Render(HDC _memDC)
{
	Vector2 vec2Position = GetPosition();
	int iWidth = m_pTexture->GetWidth();
	int iHeight = m_pTexture->GetHeight();
	vec2Position.m_ix -= iWidth / 2  ;
	vec2Position.m_iy -= iHeight / 2  ;

	TransparentBlt(_memDC, vec2Position.m_ix, vec2Position.m_iy, iWidth, iHeight,
		m_pTexture->GetDC(), 0, 0, iWidth, iHeight, RGB(255, 255, 255));
	
}

void Object::Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type)
{
	m_pTexture = ResourceManager::GetInstance()->LoadTexture(_eTexture_Type);
	SetPosition(_vec2Position);
}







