#pragma once
class GameResource
{
private:
	std::string m_strKey;
	std::string m_strRelativePath;
public:
	void SetKey(std::string _strKey) { m_strKey = _strKey; }
	void SetRelativePath(std::string _strRelativePath) { m_strRelativePath = _strRelativePath; }

	const std::string& GetKey() { return m_strKey; }
	const std::string& GetRelativePath() { return m_strRelativePath; }
	GameResource();
	~GameResource();
};

