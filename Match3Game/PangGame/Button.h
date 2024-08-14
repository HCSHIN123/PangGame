#pragma once
#include"Object.h"

class Texture;
class Button :public Object
{
private:
	bool m_bActivated;
	std::string m_strName;
	std::function<void()> m_CallBackFunction;
	Texture* m_pButtonTexture;
	Texture* m_pButtonTextureX;
	RECT	m_rect;
	bool m_bCursorOn;
	TEXTURE_TYPE m_eType;
public:
	Button();
	~Button();
	// Object을(를) 통해 상속됨
	virtual void Update() override;
	virtual void Render(HDC _memDC) override;
	virtual void Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type) override;
	virtual void Init(Vector2 _vec2Position, TEXTURE_TYPE _eTexture_Type, std::function<void()> _CallBackFunction, std::string _name);
	void SetAcivate(bool _state) { m_bActivated = _state; }
	void SetName(std::string _name) { m_strName = _name; }
	void SetRectPositionForBoard();
	bool IsCursorInRect(POINT _pt, TEXTURE_TYPE _type);
	void Render(HDC _memDC, Texture* _texture);
	Texture* GetButtonTexture() { return m_pButtonTexture; }
	TEXTURE_TYPE GetType() { return m_eType; }
	void ChangeType(TEXTURE_TYPE _type) { Init(m_BoardPosition, _type); }
};

