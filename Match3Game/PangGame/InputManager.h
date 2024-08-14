#pragma once

enum class KEY_STATE
{
	DOWN, //누른직후
	PRESS,//누르는중
	UP,//뗀직후
	NONE,//조작없음
};

struct KeyInfo
{
	KEY_STATE m_eState;
	bool	m_bPrevDown;
	KeyInfo()
	{
		m_eState = KEY_STATE::NONE;
		m_bPrevDown = false;
	}
};

class InputManager
{
	SINGLETON(InputManager)
private:
	std::map<int, KeyInfo> m_mapKeyStatus;
public:
	void Clear() { m_mapKeyStatus.clear(); }
	void Init();
	void Update();
	void RegistKey(int _iKey);
	KEY_STATE GetKeyState(int _iKey);

};


