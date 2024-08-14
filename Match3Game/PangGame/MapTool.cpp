#include "pch.h"
#include "MapTool.h"
#include "Texture.h"
#include "Core.h"
#include "InputManager.h"
#include "SceneManager.h"
void MapTool::Init()
{
	m_pBackGround = ResourceManager::GetInstance()->LoadTexture(TEXTURE_TYPE::BOARD_5X5);
	Scene::SetWindowSize(m_pBackGround->GetWidth(), m_pBackGround->GetHeight());
	m_vecBlocks.clear();

	m_vecStageBtns.resize(3);
	for (int i = 0; i < 3; ++i)
	{
		Button* button = new Button;
		button->Init({ (int)BUTTON_POSITION::START_X, (int)BUTTON_POSITION::START_Y + (int)BUTTON_POSITION::DISTANCE_Y * i },
			(TEXTURE_TYPE)(STAGE_1_BUTTON + i * 2), std::bind(&MapTool::LoadStageInfo, this, "Stage" + std::to_string(1 + i)), "Stage" + std::to_string(1 + i));
		button->SetAcivate(true);
		button->SetActivate(true);
		Scene::AddObject(button, OBJECT_GROUP::BUTTON);
		m_vecStageBtns[i] = button;
	}

	m_vecBlockSelection.resize(TEXTURE_TYPE::WALLTYPE_END - TEXTURE_TYPE::WALL);
	for (int i = 0; i < TEXTURE_TYPE::WALLTYPE_END - TEXTURE_TYPE::WALL; i++)
	{
		Button* button = new Button;
		button->Init({ (int)BUTTON_POSITION::START_X, (int)BUTTON_POSITION::START_Y + (int)BUTTON_POSITION::DISTANCE_Y * i },
			(TEXTURE_TYPE)(TEXTURE_TYPE::WALL + i),
			[=]() {m_eCurrentType = (TEXTURE_TYPE)(TEXTURE_TYPE::WALL + i); m_pCurrentBlockTexture = button->GetButtonTexture(); }, "BTN");
		button->SetActivate(false);
		button->SetAcivate(true);
		Scene::AddObject(button, OBJECT_GROUP::BUTTON);
		m_vecBlockSelection[i] = button;
	}

	m_SaveButton.Init({ (int)BUTTON_POSITION::START_X + 40, 650 },TEXTURE_TYPE::SAVE_BTN, std::bind(&MapTool::SaveStageInfo, this), "BTN");
	m_SaveButton.SetAcivate(true);
	m_SaveButton.SetActivate(false);
	
	/*m_vecBlockTypes.resize(m_iMapHeight);
	for (int i = 0; i < m_vecBlockTypes.size(); i++)
	{
		m_vecBlockTypes[i].resize(m_iMapWidth);
	}*/
	


}

void MapTool::Update()
{
	if (m_vecBlocks.size() <= 0)
	{
		Scene::Update();
		return;
	}

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(Core::GetInstance()->GethWnd(), &ptMouse);
	MouseOnCheck(ptMouse, InputManager::GetInstance()->GetKeyState(VK_LBUTTON) == KEY_STATE::UP);
	m_SaveButton.Update();
	Scene::Update();
}

void MapTool::MouseOnCheck(POINT _pt, bool _isClick)
{
	for (int y = 0; y < m_iMapHeight; ++y)
	{
		for (int x = 0; x < m_iMapWidth; ++x)
		{
			m_vecBlocks[y][x].IsCursorInRect(_pt, m_eCurrentType);
		}
	}
}

void MapTool::Render(HDC _memDC)
{
	BitBlt(_memDC, 0, 0, m_pBackGround->GetWidth(), m_pBackGround->GetHeight(), m_pBackGround->GetDC(), 0, 0, SRCCOPY);

	if (m_vecBlocks.size() <= 0)
	{
		Scene::Render(_memDC);
		return;
	}

	m_SaveButton.Render(_memDC);
	for (int y = 0; y < m_iMapHeight; ++y)
	{
		for (int x = 0; x < m_iMapWidth; ++x)
		{
			m_vecBlocks[y][x].Render(_memDC, m_pCurrentBlockTexture);
		}
	}
	TextOut(_memDC, 50, 10, m_strCurrentFileName.c_str(), m_strCurrentFileName.length());
	Scene::Render(_memDC);
}

void MapTool::LoadStageInfo(std::string _stageName)
{
	for (int i = 0; i < m_vecStageBtns.size(); i++)
	{
		m_vecStageBtns[i]->SetActivate(false);
	}
	for (int i = 0; i < m_vecBlockSelection.size(); i++)
	{
		m_vecBlockSelection[i]->SetActivate(true);
	}
	m_SaveButton.SetActivate(true);
	m_strCurrentFileName = _stageName + ".txt";
	m_Load.open(m_strCurrentFileName);
	if (m_Load.is_open())
	{
		m_Load >> m_iMapHeight;
		m_Load >> m_iMapWidth;

		m_vecBlocks.resize(m_iMapHeight);
		for (int i = 0; i < m_iMapHeight; ++i)
		{
			std::vector<Button> vecWidthBtn;
			vecWidthBtn.resize(m_iMapWidth);
			for (int j = 0; j < m_iMapWidth; ++j)
			{
				int typeNumber;
				m_Load >> typeNumber;
				if (typeNumber == 0)
					typeNumber = static_cast<int>(TEXTURE_TYPE::EMPTY_BTN);
				vecWidthBtn[j].Init({ j,i }, static_cast<TEXTURE_TYPE>(typeNumber), nullptr, " ");
				vecWidthBtn[j].SetAcivate(true);
				
				vecWidthBtn[j].SetRectPositionForBoard();
			}
			m_vecBlocks[i] = vecWidthBtn;
		}

		m_Load >> m_iChance;
		
		m_vecTargetCount.resize(static_cast<int>(TEXTURE_TYPE::PYLEE_BLOCK));

		for (int i = 0; i < m_vecTargetCount.size(); i++)
		{
			m_Load >> m_vecTargetCount[i];
		}
		m_Load.close();
	}

}

void MapTool::SaveStageInfo()
{
	m_Save.open(m_strCurrentFileName);
	if (m_Save.is_open())
	{
		m_Save.clear();
		m_Save << m_iMapHeight;
		m_Save << "\n";
		m_Save << m_iMapWidth;
		m_Save << "\n";

		std::string line;
		int type;

		for (int y = 0; y < m_iMapHeight; y++)
		{
			line.clear();
			for (int x = 0; x < m_iMapWidth; x++)
			{
				type = (int)m_vecBlocks[y][x].GetType();
				if (type == TEXTURE_TYPE::EMPTY_BTN)
					line += "0";
				else
					line += std::to_string(type);

				if(x < m_iMapWidth - 1)
					line += " ";
			}
			line += "\n";
			m_Save << line;
		}
		
		m_Save << m_iChance;
		m_Save << "\n";
		for (int i = 0; i < m_vecTargetCount.size(); i++)
		{
			m_Save << m_vecTargetCount[i];
			m_Save << "\n";
		}
		m_Save.close();
		SceneManager::GetInstance()->SceneChange(SCENE_TYPE::MAIN);
	}
}



MapTool::MapTool(std::string _strName) : Scene(_strName)
{
	m_iMapHeight = 7;
	m_iMapWidth = 7;
	m_iChance = 20;

	
}

MapTool::~MapTool()
{
}


