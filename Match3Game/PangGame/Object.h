#pragma once
#include "ResourceManager.h"

class Object
{
private:
	OBJECT_GROUP m_eType;
	Texture* m_pTexture;
	bool m_bActivated;
protected:
	Vector2				m_BoardPosition;
public:
	Object();
	virtual ~Object();

	virtual void Update() = 0;
	virtual void LateUpdate() final;//final : 자식Class에서 Overide못하게 막아주는 키워드
	virtual void Render(HDC _memDC);
	virtual void Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type);

	
	OBJECT_GROUP GetType() { return m_eType; }

	bool is_Activated() { return m_bActivated; }
	void SetActivate(bool _activate) { m_bActivated = _activate; }

	void AddPosition(Vector2 _vec2Add) { m_BoardPosition += _vec2Add; }
	void SetPosition(Vector2 _vec2Position) { m_BoardPosition = _vec2Position; }
	
	Vector2 GetPosition() { return m_BoardPosition; }
	
	
	
};

