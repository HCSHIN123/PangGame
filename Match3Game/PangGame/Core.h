#pragma once
class Core
{
	SINGLETON(Core)
private:
	HWND m_hWnd;
	HDC m_hDC;

	HBITMAP m_hBackBitMap;
	HDC m_hBackDC;
	void Update();
	void Render();
public:
	void CreateBackDC();
	void Init(HWND _hWnd);
	void GameLoop();
	HWND GethWnd() { return m_hWnd; }
	HDC GetMainDC() { return m_hDC; }
	HDC GetBackDC() { return m_hBackDC; }
};

