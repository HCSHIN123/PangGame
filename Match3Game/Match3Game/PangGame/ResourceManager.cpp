#include "pch.h"
#include "ResourceManager.h"
#include "PathManager.h"
#include "Texture.h"

ResourceManager::ResourceManager()
{

}
ResourceManager::~ResourceManager()
{
	for (auto iter = m_MapTexture.begin(); iter != m_MapTexture.end(); iter++)
		delete iter->second;
}

std::string ResourceManager::GetTextureFileName(TEXTURE_TYPE _eTextureType, DIRECTION _eDirection)
{
	switch (_eTextureType)
	{
	case TEXTURE_TYPE::BACKGROUND:						return "GreenBackGround.bmp";
	case TEXTURE_TYPE::PLAY_BUTTON:						return "PlayButton.bmp";
	case TEXTURE_TYPE::STAGE_1_BUTTON:					return "Stage1_button.bmp";
	case TEXTURE_TYPE::STAGE_2_BUTTON:					return "Stage2_button.bmp";
	case TEXTURE_TYPE::STAGE_3_BUTTON:					return "Stage3_button.bmp";
	case TEXTURE_TYPE::LOCK_BUTTON:						return "LockButton.bmp";
	case TEXTURE_TYPE::BOARD_5X5:						return "Match3Background.bmp";
	case TEXTURE_TYPE::PIKACHU_BLOCK:					return "Pikachu.bmp";
	case TEXTURE_TYPE::DEWGONG_BLOCK:					return "Dewgong.bmp";
	case TEXTURE_TYPE::JAMMANBO_BLOCK:					return "Jammanbo.bmp";
	case TEXTURE_TYPE::GGOBUGI_BLOCK:					return "Ggobugi.bmp";
	case TEXTURE_TYPE::PYLEE_BLOCK:						return "Pylee.bmp";
	case TEXTURE_TYPE::PIKACHU_SELECTED:				return "Pikachu_selected.bmp";
	case TEXTURE_TYPE::DEWGONG_SELECTED:				return "Dewgong_selected.bmp";
	case TEXTURE_TYPE::JAMMANBO_SELECTED:				return "Jammanbo_selected.bmp";
	case TEXTURE_TYPE::GGOBUGI_SELECTED:				return "Ggobugi_selected.bmp";
	case TEXTURE_TYPE::PYLEE_SELECTED:					return "Pylee_selected.bmp";
	case TEXTURE_TYPE::REMOVE_ROW_BLOCK:				return "PointerLeftRight.bmp";
	case TEXTURE_TYPE::REMOVE_COL_BLOCK:				return "PointerUpDown.bmp";
	case TEXTURE_TYPE::REMOVE_CROSS_BLOCK:				return "PointerUpDownLeftRight.bmp";
	case TEXTURE_TYPE::REMOVE_ROW_BLOCK_SELECTED:		return "PointerLeftRight_Selected.bmp";
	case TEXTURE_TYPE::REMOVE_COL_BLOCK_SELECTED:		return "PointerUpDown_Selected.bmp";
	case TEXTURE_TYPE::REMOVE_CROSS_BLOCK_SELECTED:		return "PointerUpDownLeftRight_Selected.bmp";
	case TEXTURE_TYPE::REMOVE_COLOR_BLOCK:				return "Rainbow.bmp";
	case TEXTURE_TYPE::REMOVE_COLOR_BLOCK_SELECTED:		return "Rainbow_Selected.bmp";
	case TEXTURE_TYPE::WALL:							return "Wall.bmp";
	case TEXTURE_TYPE::SCOREBOARD:						return "ScoreBoard.bmp";
	case TEXTURE_TYPE::BOOM_0:							return "00_BoomTexture.bmp";
	case TEXTURE_TYPE::BOOM_1:							return "01_BoomTexture.bmp";
	case TEXTURE_TYPE::BOOM_2:							return "02_BoomTexture.bmp";
	case TEXTURE_TYPE::BOOM_3:							return "03_BoomTexture.bmp";
	case TEXTURE_TYPE::EMPTY_BUTTON:					return "Empty.bmp";
	case TEXTURE_TYPE::USABLE_WALL:					return "CaveWall.bmp";
	case TEXTURE_TYPE::ICE_WALL:					return "IceWall.bmp";
	case TEXTURE_TYPE::EMPTY_BTN:					return "EmptyBlock.bmp";
	case TEXTURE_TYPE::SAVE_BTN:					return "SaveButton.bmp";
	case TEXTURE_TYPE::MAPTOOL_BTN:					return "MapToolButton.bmp";
	default: return "";
	}
}

Texture* ResourceManager::LoadTexture(TEXTURE_TYPE _eTextureType, DIRECTION _eDirection)
{
	std::string strFileName = GetTextureFileName(_eTextureType, _eDirection);
	assert(strFileName.length() != 0);

	std::string strKey = strFileName.substr(0, strFileName.length() - 4);

	Texture* pTexture = FindTexture(strKey);
	if (pTexture == nullptr)
	{
		pTexture = new Texture;
		std::string strPath = PathManager::GetInstance()->GetContentpath();
		strPath += ConstValue::strTexturePath + strFileName;
		pTexture->Load(strPath);
		pTexture->SetKey(strKey);
		pTexture->SetRelativePath(strFileName);
		m_MapTexture.insert(std::make_pair(strKey, pTexture));
	}
	return pTexture;
}

Texture* ResourceManager::FindTexture(const std::string& _strKey)
{
	std::map<std::string, Texture*>::iterator iter = m_MapTexture.find(_strKey);
	if (iter == m_MapTexture.end())
		return nullptr;
	else
		return iter->second;
}
