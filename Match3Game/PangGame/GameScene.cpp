#include "pch.h"
#include "GameScene.h"
#include "Texture.h"
#include "TimerManager.h"
#include "User.h"


void GameScene::Init()
{

	int iBoardHeight;
	int iBoardWidth;
	std::vector<std::vector<int>> vecBoardInfo;
	int iChance;
	std::vector<GameGoal> vecScoreBoard;

	std::ifstream load;
	std::string strstage = User::GetInstance()->getPlayingStageName();
	load.open(strstage + ".txt");
	if (load.is_open())
	{
		load >> iBoardHeight;
		load >> iBoardWidth;

		vecBoardInfo.resize(iBoardHeight);
		for (int i = 0; i < iBoardHeight; ++i)
		{
			std::vector<int> vecWidth(iBoardWidth);
			for (int j = 0; j < iBoardWidth; ++j)
			{
				int typeNumber;
				load >> typeNumber;
				

				vecWidth[j] = typeNumber;
			}
			vecBoardInfo[i] = vecWidth;
		}

		load >> iChance;
		SetGameGoal(&load, vecScoreBoard);
		m_Board = new Board;
		m_Board->Init(TEXTURE_TYPE::BOARD_5X5, vecBoardInfo,iChance, vecScoreBoard);
		Scene::SetWindowSize(m_Board->GetTexture()->GetWidth(), m_Board->GetTexture()->GetHeight());
		Scene::AddObject(m_Board, OBJECT_GROUP::BOARD);
		load.close();
	}


}


void GameScene::Update()
{
	Scene::Update();
}

void GameScene::Render(HDC _memDC)
{
	Scene::Render(_memDC);
}

void GameScene::SetGameGoal(std::ifstream* _load, std::vector<GameGoal>& m_vecScoreBoard)
{
	TEXTURE_TYPE blocks[ALL_NORMAL_BLOCKTYPE] = { PIKACHU_BLOCK,DEWGONG_BLOCK,JAMMANBO_BLOCK,GGOBUGI_BLOCK,PYLEE_BLOCK };
	Vector2 RenderStartPosition = { 40, 2 };
	int ydistance = 8;


	for (int i = 0; i < ALL_NORMAL_BLOCKTYPE; ++i)
	{
		Texture* texture = ResourceManager::GetInstance()->LoadTexture(blocks[i]);
		RECT m_rect;
		m_rect.left = RenderStartPosition.m_ix;
		m_rect.top = (RenderStartPosition.m_iy + i) * texture->GetHeight();
		m_rect.right = m_rect.left + texture->GetWidth();
		m_rect.bottom = m_rect.top + texture->GetHeight();
		int itargetScore;
		*_load >> itargetScore;

		m_vecScoreBoard.push_back(GameGoal(texture, itargetScore, m_rect));
	}
}

GameScene::GameScene(std::string _strName) : Scene(_strName)
{
	
}

