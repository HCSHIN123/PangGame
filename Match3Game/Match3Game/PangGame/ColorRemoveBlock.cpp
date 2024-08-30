//#include "pch.h"
//#include "Block.h"
//#include "Texture.h"
//#include "InputManager.h"
//#include "Core.h"
//#include "SceneManager.h"
//#include "ColorRemoveBlock.h"
//#include "Board.h"
//
//void ColorRemoveBlock::Reset(Vector2 _position, TEXTURE_TYPE _texture, std::function<void(Block*)> _CallBackFunction)
//{
//	Block::SetTextureType(_texture, ALL_COLORREMOVE_BLOCKTYPE);
//	SetCallBackFunc(_CallBackFunction);
//	Block::SetPosition(_position);
//	m_CallBackFunction = _CallBackFunction;
//}
//
//void ColorRemoveBlock::SetCallBackFunc(std::function<void(Block*)> _CallBackFunction)
//{
//	m_CallBackFunction = _CallBackFunction;
//}
//
//bool ColorRemoveBlock::Check()
//{
//	m_CallBackFunction(this);
//	return true;
//}
