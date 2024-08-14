#include "pch.h"
#include "InputManager.h"
#include "Core.h"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::Init()
{
}

void InputManager::Update()
{
	HWND hMainWnd = Core::GetInstance()->GethWnd();
	//포커싱된 윈도우 핸들값
	HWND hWnd = GetFocus();
	if (hWnd == hMainWnd)
	{
		for (std::map<int, KeyInfo>::iterator iter = m_mapKeyStatus.begin(); iter != m_mapKeyStatus.end(); ++iter)
		{
			if (GetAsyncKeyState(iter->first) & 0x8000)
			{
				if (iter->second.m_bPrevDown)
				{
					iter->second.m_eState = KEY_STATE::PRESS;
				}
				else
				{
					iter->second.m_eState = KEY_STATE::DOWN;
				}
				iter->second.m_bPrevDown = true;
			}
			else
			{
				if (iter->second.m_bPrevDown)
				{
					iter->second.m_eState = KEY_STATE::UP;
				}
				else
				{
					iter->second.m_eState = KEY_STATE::NONE;
				}
				iter->second.m_bPrevDown = false;
			}
		}
	}
	else
	{
		for (std::map<int, KeyInfo>::iterator iter = m_mapKeyStatus.begin(); iter != m_mapKeyStatus.end(); ++iter)
		{
			iter->second.m_bPrevDown = false;
			if (iter->second.m_eState == KEY_STATE::DOWN || iter->second.m_eState == KEY_STATE::PRESS)
				iter->second.m_eState = KEY_STATE::UP;
			else if (iter->second.m_eState == KEY_STATE::UP)
				iter->second.m_eState = KEY_STATE::NONE;
		}
	}
}

void InputManager::RegistKey(int _iKey)
{
	if (m_mapKeyStatus.find(_iKey) != m_mapKeyStatus.end())
		return;
	KeyInfo keyInfo;
	m_mapKeyStatus.insert(std::make_pair(_iKey, keyInfo));
}

KEY_STATE InputManager::GetKeyState(int _iKey)
{
	std::map<int, KeyInfo>::iterator findIter = m_mapKeyStatus.find(_iKey);
	assert(findIter != m_mapKeyStatus.end());

	return findIter->second.m_eState;
}
