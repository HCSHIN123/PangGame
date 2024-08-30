#pragma once
#include"GameResource.h"
class Texture : public GameResource
{
private:
	HDC m_hDC;
	HBITMAP m_hBitMap;
	BITMAP m_BitMapInfomation;
public:
	void Load(std::string _strFilePath);
	HDC GetDC() { return m_hDC; }
	int GetWidth() { return m_BitMapInfomation.bmWidth; }
	int GetHeight() { return m_BitMapInfomation.bmHeight; }
	Texture();
	~Texture();
};

