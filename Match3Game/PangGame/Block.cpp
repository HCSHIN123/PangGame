#include "pch.h"
#include "Block.h"
#include "Texture.h"
#include "InputManager.h"
#include "Core.h"
#include "SceneManager.h"
#include "TimerManager.h"
#include "Board.h"


Block::Block()
{
	m_rect = {};
	m_bSelect = false;
	m_pBlockTexture = nullptr;
	m_pSelectBlockTexture = nullptr;
	m_fWeight = 0.0f;
	m_bisBoom = false;
	Object::SetActivate(false);
	InitAnimation();
	m_fDropSpeed = 3.0f;
	m_fBreakWeight = 0.4f;
}

Block::~Block()
{
}

void Block::Update()
{
	if (Object::is_Activated() == false || m_eBlockType == WALL)
		return;

	if (m_bisBoom)
	{
		m_Animation.Update();
		return;
	}

	if (InputManager::GetInstance()->GetKeyState(VK_LBUTTON) == KEY_STATE::DOWN)
	{
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(Core::GetInstance()->GethWnd(), &ptMouse);

		if (PtInRect(&m_rect, ptMouse) == true)
		{	
			Select();
			m_GameBoard->SelectBlock(this);
		}
	}
}

void Block::Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type, Board* _board, std::function<bool(Block*)> _CallBackFunction)
{
	Reset(_vec2Position, _eTexture_Type, _CallBackFunction);
	m_GameBoard = _board;
	SetBlockID(BLOCK_ID::NORMAL_BLOCK);
}

void Block::InitAnimation()
{
	std::vector<AnimNode> vecAnimList;
	for (int i = TEXTURE_TYPE::BOOM_0; i < TEXTURE_TYPE::BOOM_END; ++i)
	{
		Texture* animTexture = ResourceManager::GetInstance()->LoadTexture(static_cast<TEXTURE_TYPE>(i));
		AnimNode animNode(animTexture);

		if (i == BOOM_END - 1)
		{
			animNode.m_callBack = [=]()
				{
					m_bisBoom = false;
				};
		}
		vecAnimList.push_back(animNode);
	}

	m_Animation.Init(vecAnimList, ANIMATION_TYPE::ONCE, 0.15f);
}

void Block::Reset(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type)
{
	m_bSelect = false;
	SetTextureType(_eTexture_Type);
	SetPosition(_vec2Position);
	Object::SetActivate(true);
	m_TargetPosition = _vec2Position;
	m_Animation.Reset();
}

void Block::Reset(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type, std::function<bool(Block*)> _CallBackFunction)
{
	Reset(_vec2Position, _eTexture_Type);
	m_CallBackFunction = _CallBackFunction;
}

void Block::SetPosition(Vector2 _vec2Position)
{
	m_BoardPosition = _vec2Position;
	SetRenderPosition({ (float)m_BoardPosition.m_ix,(float)m_BoardPosition.m_iy});
}

bool Block::Check()
{
	return m_CallBackFunction(this);
}

void Block::Boom()
{
	m_Animation.Reset();
	m_bisBoom = true;
}

void Block::SetTextureType(TEXTURE_TYPE _type)
{
	m_eBlockType = _type; 
	m_pBlockTexture = ResourceManager::GetInstance()->LoadTexture(m_eBlockType);

	if(_type != WALL)
		m_pSelectBlockTexture = ResourceManager::GetInstance()->LoadTexture(TEXTURE_TYPE(_type + ALL_BLOCKTYPE));
}

void Block::SetRectPosition()
{
	m_rect.left =(BOARD_START::X + m_fRenderPosition.m_fx * m_pBlockTexture->GetWidth());
	m_rect.top = (BOARD_START::Y + m_fRenderPosition.m_fy * m_pBlockTexture->GetHeight());
	m_rect.right = m_rect.left + m_pBlockTexture->GetWidth();
	m_rect.bottom = m_rect.top + m_pBlockTexture->GetHeight();
}

void Block::SetRenderPosition(RenderPosition _fposition)
{
	m_fRenderPosition = _fposition;
	SetRectPosition();
}

bool Block::DropPosition()
{
	// 선형보간으로 블럭 떨어지기

	if (m_fRenderPosition.m_fy < static_cast<float>(m_TargetPosition.m_iy))
	{
		m_fWeight += TimerManager::GetInstance()->GetfDeltaTime() * m_fDropSpeed;
		m_fWeight = m_fWeight < 1.0f ? m_fWeight : 1.0f;

		m_fRenderPosition.m_fy = static_cast<float>(m_BoardPosition.m_iy) +
			(static_cast<float>(m_TargetPosition.m_iy) - static_cast<float>(m_BoardPosition.m_iy)) * m_fWeight;
		SetRectPosition();

		return false;
	}

	SetRenderPosition({ static_cast<float>(m_TargetPosition.m_ix), static_cast<float>(m_TargetPosition.m_iy) });
	m_BoardPosition = m_TargetPosition;
	m_fWeight = 0.0f;
	return true;
}

bool Block::MovePosition()
{
	RenderPosition targetRenderPos((float)m_TargetPosition.m_ix, (float)m_TargetPosition.m_iy);
	RenderPosition dir = targetRenderPos - m_fRenderPosition;
	if (abs(dir.m_fx) >= 0.15f || abs(dir.m_fy) >= 0.15f)
	{
		m_fWeight += TimerManager::GetInstance()->GetfDeltaTime() * m_fBreakWeight;
		m_fWeight = m_fWeight < 1.0f ? m_fWeight : 1.0f;

		m_fRenderPosition.m_fy = m_fRenderPosition.m_fy +
			dir.m_fy * m_fWeight;
		m_fRenderPosition.m_fx = m_fRenderPosition.m_fx +
			dir.m_fx * m_fWeight;

		SetRectPosition();

		return false;
	}

	SetRenderPosition({ static_cast<float>(m_TargetPosition.m_ix), static_cast<float>(m_TargetPosition.m_iy) });
	m_BoardPosition = m_TargetPosition;
	m_fWeight = 0.0f;
	return true;
}

void Block::Render(HDC _hDC)
{
	if (Object::is_Activated() == false)
		return;

	if (m_bisBoom == true)
	{
		m_Animation.Render(_hDC, {m_rect.left, m_rect.top});
		return;
	}

	if (m_pSelectBlockTexture != nullptr && m_bSelect == true)
		TransparentBlt(_hDC, m_rect.left, m_rect.top, m_pSelectBlockTexture->GetWidth(), m_pSelectBlockTexture->GetHeight(), m_pSelectBlockTexture->GetDC(),
			0, 0, m_pSelectBlockTexture->GetWidth(), m_pSelectBlockTexture->GetHeight(), RGB(255, 0, 255));
	else
		TransparentBlt(_hDC, m_rect.left, m_rect.top, m_pBlockTexture->GetWidth(), m_pBlockTexture->GetHeight(), m_pBlockTexture->GetDC(),
			0, 0, m_pBlockTexture->GetWidth(), m_pBlockTexture->GetHeight(), RGB(255, 0, 255));
}
	
