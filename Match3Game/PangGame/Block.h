#pragma once
#include"ResourceManager.h"
#include"Object.h"
#include"Animation.h"

class Board;
class Block : public Object
{
protected:
	TEXTURE_TYPE		m_eBlockType;
	bool				m_bSelect;
	Texture*		    m_pBlockTexture;
	Texture*			m_pSelectBlockTexture;

	Board*				m_GameBoard;
	Vector2				m_TargetPosition;
	RenderPosition		m_fRenderPosition;
	float				m_fWeight;
	float				m_fDropSpeed;
	float				m_fBreakWeight;
	RECT				m_rect;

	Animation			m_Animation;
	bool				m_bisBoom;

	std::function<bool(Block*)> m_CallBackFunction;
	
public:
	Block();
	~Block();
	virtual void Update();
	virtual void Render(HDC _memDC);
	void Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type, Board* _board, std::function<bool(Block*)> _CallBackFunction);
	void InitAnimation();
	void Reset(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type);
	void Reset(Vector2 _position, TEXTURE_TYPE _texture, std::function<bool(Block*)> _CallBackFunction);
	

	void SetPosition(Vector2 _vec2Position);
	bool Check();
	void Boom();
	bool isBoom() { return m_bisBoom; }
	void SetTextureType(TEXTURE_TYPE _type);
	virtual TEXTURE_TYPE getBlockType() { return m_eBlockType; }
	
	void SetRectPosition();
	void SetRenderPosition(RenderPosition _fposition);
	void SetNewPosition(Vector2 _position) { m_TargetPosition = _position;  }
	bool DropPosition();
	bool MovePosition();
	
	void Select() { m_bSelect = true; };
	void Cancle() { m_bSelect = false; };

	Vector2 GetBoardPosition() { return m_BoardPosition; }
	Texture* GetTexture() { return m_pBlockTexture; }
};
