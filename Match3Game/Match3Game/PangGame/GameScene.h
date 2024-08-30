#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "Board.h"
#include <fstream>
enum BOARD_SIZE
{
	HEIGHT = 5,
	WIDTH = 5,
};


class GameScene : public Scene
{
private:
	Board* m_Board;
	std::string m_strCurrentStageFileName;
	
public:
	
	Board* GetBoard() { return m_Board; }
	virtual void Update() override;
	virtual void Render(HDC _memDC) override;
	//virtual void Init() {};
	virtual void Init();
	
	void SetStage(std::string _stagename) { m_strCurrentStageFileName = _stagename; }
	void SetGameGoal(std::ifstream* _load, std::vector<GameGoal>& m_vecScoreBoard);
	GameScene(std::string _strName);
};

