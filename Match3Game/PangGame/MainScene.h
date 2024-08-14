#pragma once
#include"Scene.h"
#include"ResourceManager.h"
#include "Button.h"
class MainScene : public Scene
{
private:
	Texture* m_pBackGround;
	Vector2 m_vec2BackGroundPosition;
	std::vector<Button*> m_vecStageButton;
	Button* m_pPlayButton;
	Button* m_pPlayMapToolButton;
public:
	MainScene(std::string _strName);
	// Scene을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render(HDC _memDC) override;
	void StartGame();
	void StartMapTool();
	void SelectStage();
};

