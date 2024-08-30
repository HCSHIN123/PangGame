#pragma once
#include<fstream>
class User
{
	SINGLETON(User)

	
private:
	int m_iCurrentLevel;
	
	std::string m_strCurrentPlay;
	std::ifstream m_Load;
	std::ofstream m_Save;
public:
	void LevelUp() { if(m_iCurrentLevel < (int)MAX_LEVEL) ++m_iCurrentLevel; }
	void Init();
	int getLevel() { return m_iCurrentLevel; }
	std::string getPlayingStageName() { return m_strCurrentPlay; }
	void SetPlayingStageName(std::string _name) { m_strCurrentPlay = _name; }
	void StageClear();
};

