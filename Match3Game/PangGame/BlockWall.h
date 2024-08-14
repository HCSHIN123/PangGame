#pragma once
#include "Block.h"
#include "Texture.h"
class BlockWall : public Block
{
private:
	TEXTURE_TYPE m_eTextureType;
	Texture* m_pWallTexture;
public:
	BlockWall();
	~BlockWall();
	virtual void Update() override;
	virtual void Render(HDC _memDC) override;
	virtual void Reset(TEXTURE_TYPE _newType);
	TEXTURE_TYPE GetWallType() { return m_eTextureType; }
	 /*TEXTURE_TYPE getBlockType()
	{ 
		if(m_pWallTexture != nullptr)
			return m_eTextureType; 
		else
			return m_eBlockType; 
	}*/
	void InitWall(TEXTURE_TYPE _eWallTexture);
};

