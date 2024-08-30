#pragma once
#include "ResourceManager.h"

class Object
{
private:
	OBJECT_GROUP m_eType;	// Object종류
	Texture* m_pTexture;	// 텍스쳐
	bool m_bActivated;		// 활성화 여부
protected:
	Vector2				m_BoardPosition;	// 위치
public:
	Object();
	virtual ~Object();
	virtual void Update() = 0;
	virtual void LateUpdate() final;
	virtual void Render(HDC _memDC);
	virtual void Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type);
	
	OBJECT_GROUP GetType() { return m_eType; }

	bool is_Activated() { return m_bActivated; }
	void SetActivate(bool _activate) { m_bActivated = _activate; }

	void AddPosition(Vector2 _vec2Add) { m_BoardPosition += _vec2Add; }
	void SetPosition(Vector2 _vec2Position) { m_BoardPosition = _vec2Position; }
	
	Vector2 GetPosition() { return m_BoardPosition; }
};

