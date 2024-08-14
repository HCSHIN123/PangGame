#pragma once
class PathManager
{
	SINGLETON(PathManager)
private:
	char m_szContentsPath[512];
public:
	void Init();
	const char* GetContentpath() { return m_szContentsPath; }
};

