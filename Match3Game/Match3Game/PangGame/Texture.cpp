#include "pch.h"
#include "Texture.h"
#include "Core.h"

Texture::Texture()
{
	m_BitMapInfomation = {};
	m_hBitMap = nullptr;
	m_hDC = nullptr;
}

Texture::~Texture()
{
}

void Texture::Load(std::string _strFilePath)
{
	m_hBitMap = (HBITMAP)LoadImage(nullptr, _strFilePath.c_str(), IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (m_hBitMap == nullptr)
	{
		m_hBitMap = nullptr;
	}
	assert(m_hBitMap != nullptr);

	m_hDC = CreateCompatibleDC(Core::GetInstance()->GetMainDC());

	HBITMAP hPrevBitMap = (HBITMAP)SelectObject(m_hDC, m_hBitMap);
	DeleteObject(hPrevBitMap);

	GetObject(m_hBitMap, sizeof(BITMAP), &m_BitMapInfomation);
}

