//#include "pch.h"
//#include "Block.h"
//#include "Texture.h"
//#include "InputManager.h"
//#include "Core.h"
//#include "SceneManager.h"
//#include "LineRemoveBlock.h"
//#include "Board.h"
//
//void LineRemoveBlock::Reset(Vector2 _position, TEXTURE_TYPE _texture, std::function<bool(Block*)> _CallBackFunction)
//{
//	Block::SetTextureType(_texture, ALL_LINEREMOVE_BLOCKTYPE);
//	Block::SetPosition(_position);
//	SetCallBackFunc(_CallBackFunction);
//	m_CallBackFunction = _CallBackFunction;
//}
//
//void LineRemoveBlock::SetCallBackFunc(std::function<bool(Block*)> _CallBackFunction)
//{
//	m_CallBackFunction = _CallBackFunction;
//}
//
//bool LineRemoveBlock::Check()
//{
//	m_CallBackFunction(this);
//	return true;
//}
