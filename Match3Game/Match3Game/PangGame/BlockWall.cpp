#include"pch.h"
#include "BlockWall.h"

BlockWall::BlockWall()
{
}

BlockWall::~BlockWall()
{
}

void BlockWall::Update()
{
	if (m_bisBoom)
	{
		m_Animation.Update();
		return;
	}
	return;
}

void BlockWall::Render(HDC _memDC)
{
	if (Object::is_Activated() == false)
		return;
	if (m_bisBoom == true)
	{
		m_Animation.Render(_memDC, { m_rect.left, m_rect.top });
	}
	else
	TransparentBlt(_memDC, m_rect.left, m_rect.top, m_pBlockTexture->GetWidth(), m_pBlockTexture->GetHeight(), m_pBlockTexture->GetDC(),
		0, 0, m_pBlockTexture->GetWidth(), m_pBlockTexture->GetHeight(), RGB(255, 0, 255));
	if(m_pWallTexture != nullptr)
		TransparentBlt(_memDC, m_rect.left, m_rect.top, m_pWallTexture->GetWidth(), m_pWallTexture->GetHeight(), m_pWallTexture->GetDC(),
			0, 0, m_pWallTexture->GetWidth(), m_pWallTexture->GetHeight(), RGB(255, 0, 255));
	
}

void BlockWall::Reset(TEXTURE_TYPE _newType)
{
	m_eBlockType = _newType;
	m_pBlockTexture = ResourceManager::GetInstance()->LoadTexture(_newType);
}

void BlockWall::InitWall(TEXTURE_TYPE _eWallTexture)
{
	m_eTextureType = _eWallTexture;
	m_pWallTexture = ResourceManager::GetInstance()->LoadTexture(_eWallTexture);
	SetBlockID(BLOCK_ID::WALL_BLOCK);
}
