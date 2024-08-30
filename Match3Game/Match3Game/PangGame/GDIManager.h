#pragma once

enum class BRUSH_TYPE
{
	HOLLOW,
	RED,
	BLUE,
	GREEN,
	BLACK,
	WHITE,
	END,
};

enum class PEN_TYPE
{
	RED,
	BLUE,
	GREEN,
	BLACK,
	WHITE,
	END,
};
class GDIManager
{
	SINGLETON(GDIManager)
private:
	HBRUSH m_BrushList[static_cast<int>(BRUSH_TYPE::END)];
	HPEN m_PenList[static_cast<int>(PEN_TYPE::END)];
	HBRUSH m_PrevBrush;
	HPEN m_PrevPen;
public:
	void CreateMyPen();
	void ReleaeMyPen();
	void CreateMyBrush();
	void ReleaseMyBrush();
	void SetBrush(HDC _hDC, BRUSH_TYPE _eBrushType)
	{
		m_PrevBrush = (HBRUSH)SelectObject(_hDC, m_BrushList[static_cast<int>(_eBrushType)]);
	}
	void ResetBrush(HDC _hDC)
	{
		if (m_PrevBrush != nullptr)
			SelectObject(_hDC, m_PrevBrush);
	}
	void SetPen(HDC _hDC, PEN_TYPE _ePenType)
	{
		m_PrevPen = (HPEN)SelectObject(_hDC, m_PenList[static_cast<int>(_ePenType)]);
	}
	void ResetPen(HDC _hDC)
	{
		if (m_PrevPen != nullptr)
			SelectObject(_hDC, m_PrevPen);
	}
};

