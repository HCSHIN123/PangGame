#include "pch.h"
#include "MainScene.h"
#include "Texture.h"
#include"SceneManager.h"
#include "InputManager.h"
#include"User.h"
 
static bool isBegin;

MainScene::MainScene(std::string _strName) : Scene(_strName)
{
	m_pBackGround = nullptr;
	isBegin = true;
}



void MainScene::Init()
{
	
	m_pBackGround = ResourceManager::GetInstance()->LoadTexture(TEXTURE_TYPE::BACKGROUND);
	Scene::SetWindowSize(m_pBackGround->GetWidth(), m_pBackGround->GetHeight());

	if (isBegin) // 첫실행때 플레이버튼 활성화
	{
		Button* PlayButton = new Button;
		m_pPlayButton = PlayButton;
		PlayButton->Init({ 380  ,170 }, TEXTURE_TYPE::PLAY_BUTTON, [&]()
			{
				SelectStage(); 
				m_pPlayButton->SetAcivate(false);
			}
		, "Play");
		PlayButton->SetAcivate(true);
		Scene::AddObject(PlayButton, OBJECT_GROUP::BUTTON);
		isBegin = false;
	}
	else // 아닌경우 스테이지 선택버튼을 활성화
		SelectStage();

#ifdef _DEBUG
	Button* m_pPlayMapToolButton = new Button;
	m_pPlayMapToolButton->Init({ 770  ,20 }, TEXTURE_TYPE::MAPTOOL_BTN, std::bind(&MainScene::StartMapTool, this), "Play");
	m_pPlayMapToolButton->SetAcivate(true);
	Scene::AddObject(m_pPlayMapToolButton, OBJECT_GROUP::BUTTON);
	isBegin = true;
#endif
}

void MainScene::Update()
{
	Scene::Update();
}

void MainScene::Render(HDC _memDC)
{
	BitBlt(_memDC, 0, 0, m_pBackGround->GetWidth(), m_pBackGround->GetHeight(), m_pBackGround->GetDC(), 0, 0, SRCCOPY);
	Scene::Render(_memDC);
}


void MainScene::StartGame()
{
	
	SceneManager::GetInstance()->SceneChange(SCENE_TYPE::GAME_SCENE);
}

void MainScene::StartMapTool()
{
	SceneManager::GetInstance()->SceneChange(SCENE_TYPE::MAP_TOOL);
}

void MainScene::SelectStage()
{

	m_vecStageButton.resize((int)MAX_LEVEL);

	for (int i = 0; i < m_vecStageButton.size(); ++i)
	{
		Button* button = new Button;
		m_vecStageButton[i] = button;
		button->Init({ (int)BUTTON_POSITION::START_X, (int)BUTTON_POSITION::START_Y + (int)BUTTON_POSITION::DISTANCE_Y * i },
			(TEXTURE_TYPE)(STAGE_1_BUTTON + i * 2),	std::bind(&MainScene::StartGame, this), "Stage" + std::to_string(1 + i));
		Scene::AddObject(button, OBJECT_GROUP::BUTTON);
	}

	for (int i = 0; i < User::GetInstance()->getLevel(); ++i)
	{
		m_vecStageButton[i]->SetAcivate(true);
	}
}
