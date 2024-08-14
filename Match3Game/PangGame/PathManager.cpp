#include "pch.h"
#include "PathManager.h"

PathManager::PathManager() : m_szContentsPath{}
{

}
PathManager::~PathManager()
{

}
void PathManager::Init()
{
	GetCurrentDirectory(sizeof(m_szContentsPath), m_szContentsPath);
	int lenth = strlen(m_szContentsPath);
	int i;
	for (i = lenth - 1; m_szContentsPath[i] != '\\'; --i);
	m_szContentsPath[i] = NULL;
	strcat_s(m_szContentsPath, "\\Release\\Resource\\");
}
