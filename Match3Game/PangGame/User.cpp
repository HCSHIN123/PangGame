#include"pch.h"
#include "User.h"


User::User()
{
	m_iCurrentLevel = 0;
	m_strCurrentPlay = "\0";
}

User::~User()
{
	if (m_Load.is_open())
		m_Load.close();

	m_Save.open("User.txt");
	if (m_Save.is_open())
	{
		m_Save << m_iCurrentLevel;

		m_Save.close();
	}
}

void User::Init()
{
	m_Load.open("User.txt");
	if (m_Load.is_open())
	{
		m_Load >> m_iCurrentLevel;

		if (m_iCurrentLevel > (int)MAX_LEVEL || m_iCurrentLevel < 1)
			m_iCurrentLevel = 1;

		m_Load.close();
	}
}

void User::StageClear()
{
	char cNumber = m_strCurrentPlay.back();
	int icurrent = cNumber - '0';
	
	if (m_iCurrentLevel == icurrent)
		LevelUp();
	
}
