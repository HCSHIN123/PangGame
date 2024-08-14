#include "pch.h"
#include "Core.h"
#include "InputManager.h"
#include "PathManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TimerManager.h"
#include "User.h"
 
Core::Core()
{
	m_hBackBitMap = nullptr;
	m_hBackDC = nullptr;
	m_hDC = nullptr;
	m_hWnd = nullptr;
}
Core::~Core()
{
	if (m_hBackBitMap != nullptr)
		DeleteObject(m_hBackBitMap);
	if (m_hBackDC != nullptr)
		DeleteDC(m_hBackDC);
	if (m_hWnd != nullptr && m_hDC != nullptr)
		ReleaseDC(m_hWnd, m_hDC);
}

void Core::Update()
{
	TimerManager::GetInstance()->Update();
	InputManager::GetInstance()->Update();
	SceneManager::GetInstance()->Update();

}

void Core::Render()
{
	if (SceneManager::GetInstance()->GetCurScene() == nullptr)
		return;
	SceneManager::GetInstance()->Render(m_hBackDC);
	Vector2 WindowSize = SceneManager::GetInstance()->GetCurScene()->GetWindowSize();
	BitBlt(m_hDC, 0, 0, WindowSize.m_ix, WindowSize.m_iy, m_hBackDC, 0, 0, SRCCOPY);
}

void Core::Init(HWND _hWnd)
{
	m_hWnd = _hWnd;
	m_hDC = GetDC(_hWnd);
	User::GetInstance()->Init();
	TimerManager::GetInstance()->Init();
	InputManager::GetInstance()->Init();
	InputManager::GetInstance()->RegistKey(VK_LBUTTON);

	PathManager::GetInstance()->Init();
	SceneManager::GetInstance()->Init();
	CreateBackDC();
}

void Core::CreateBackDC()
{
	if (m_hBackBitMap != nullptr)
		DeleteObject(m_hBackBitMap);
	if (m_hBackDC != nullptr)
		DeleteDC(m_hBackDC);
	if (SceneManager::GetInstance()->GetCurScene() != nullptr)
	{
		Vector2 WindowSize = SceneManager::GetInstance()->GetCurScene()->GetWindowSize();
		m_hBackBitMap = CreateCompatibleBitmap(m_hDC, WindowSize.m_ix, WindowSize.m_iy);
		m_hBackDC = CreateCompatibleDC(m_hDC);
		HBITMAP hOldBitMap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitMap);
		DeleteObject(hOldBitMap);
	}
}
void Core::GameLoop()
{
	Update();
	Render();
}
