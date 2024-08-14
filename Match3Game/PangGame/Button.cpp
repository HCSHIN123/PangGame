#include "pch.h"
#include "Button.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "InputManager.h"
#include "Core.h"
#include "User.h"

Button::Button()
{
	m_pButtonTexture = nullptr;
	m_CallBackFunction = nullptr;
	m_bActivated = false;
	m_bCursorOn = false;
}

Button::~Button()
{
}

void Button::Update()
{
	if (!m_bActivated || !Object::is_Activated())
		return;

	if (InputManager::GetInstance()->GetKeyState(VK_LBUTTON) == KEY_STATE::DOWN)
	{
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(Core::GetInstance()->GethWnd(), &ptMouse);
		if (m_CallBackFunction != nullptr && PtInRect(&m_rect, ptMouse) == true)
		{
			User::GetInstance()->SetPlayingStageName(m_strName);
			m_CallBackFunction();
		}
	}
}

void Button::Render(HDC _memDC)
{
	if (m_pButtonTexture == nullptr || !Object::is_Activated())
		return;
	if (m_bActivated == true)
		TransparentBlt(_memDC, m_rect.left, m_rect.top, m_pButtonTexture->GetWidth(), m_pButtonTexture->GetHeight(), m_pButtonTexture->GetDC(),
			0, 0, m_pButtonTexture->GetWidth(), m_pButtonTexture->GetHeight(), RGB(255, 0, 255));
	else
		TransparentBlt(_memDC, m_rect.left, m_rect.top, m_pButtonTextureX->GetWidth(), m_pButtonTextureX->GetHeight(), m_pButtonTextureX->GetDC(),
			0, 0, m_pButtonTextureX->GetWidth(), m_pButtonTextureX->GetHeight(), RGB(255, 0, 255));
}

void Button::Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type)
{
	m_pButtonTexture = ResourceManager::GetInstance()->LoadTexture(_eTexture_Type);
	m_eType = _eTexture_Type;
	_eTexture_Type == TEXTURE_TYPE::PLAY_BUTTON ?
		m_pButtonTextureX = ResourceManager::GetInstance()->LoadTexture((TEXTURE_TYPE::EMPTY_BUTTON)) :
		m_pButtonTextureX = ResourceManager::GetInstance()->LoadTexture((TEXTURE_TYPE::LOCK_BUTTON));

	Object::SetPosition(_vec2Position);
	m_rect.left = _vec2Position.m_ix;
	m_rect.top = _vec2Position.m_iy;
	m_rect.right = _vec2Position.m_ix + m_pButtonTexture->GetWidth();
	m_rect.bottom = _vec2Position.m_iy + m_pButtonTexture->GetHeight();
}

void Button::Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type, std::function<void()> _CallBackFunction, std::string _name)
{
	Init(_vec2Position, _eTexture_Type);
	SetName(_name);

	m_CallBackFunction = _CallBackFunction;
}

void Button::SetRectPositionForBoard()
{
	RECT rect;
	rect.left = (480 + m_BoardPosition.m_ix * 95);
	rect.top = (65 + m_BoardPosition.m_iy * 95);
	rect.right = rect.left + 95;
	rect.bottom = rect.top + 95;
	m_rect = rect;
}

bool Button::IsCursorInRect(POINT _pt, TEXTURE_TYPE _type)
{
	if (PtInRect(&m_rect, _pt))
	{
		m_bCursorOn = true;
		if (InputManager::GetInstance()->GetKeyState(VK_LBUTTON) == KEY_STATE::UP)
		{
			Init(m_BoardPosition, _type);
			SetRectPositionForBoard();
		}
	}
	else
	{
		m_bCursorOn = false;
	}

	return PtInRect(&m_rect,_pt);
}

void Button::Render(HDC _memDC, Texture* _texture)
{
	if (!m_bCursorOn || _texture == nullptr)
	{
		Render(_memDC);
		return;
	}

	TransparentBlt(_memDC, m_rect.left, m_rect.top, _texture->GetWidth(), _texture->GetHeight(), _texture->GetDC(),
		0, 0, _texture->GetWidth(), _texture->GetHeight(), RGB(255, 0, 255));
}
