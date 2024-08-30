#include "pch.h"
#include "SceneManager.h"
#include "Core.h"
#include "Scene.h"
#include "MainScene.h"
#include "GameScene.h"
#include "MapTool.h"
SceneManager::SceneManager() : m_arrScene{}
{
	m_eCurScene = SCENE_TYPE::END;
}
SceneManager::~SceneManager()
{
	for (int i = 0; i < static_cast<int>(SCENE_TYPE::END); i++)
	{
		if (m_arrScene[i] != nullptr)
		{
			m_arrScene[i]->Release();
			delete m_arrScene[i];
		}
	}
}
void SceneManager::Init()
{
	m_arrScene[static_cast<int>(SCENE_TYPE::MAIN)] = new MainScene("Play");
	m_arrScene[static_cast<int>(SCENE_TYPE::GAME_SCENE)] = new GameScene("GameScene");
	m_arrScene[static_cast<int>(SCENE_TYPE::MAP_TOOL)] = new MapTool("MapTool");
	SceneChange(SCENE_TYPE::MAIN);
}

void SceneManager::Update()
{
	if (m_eCurScene == SCENE_TYPE::END || m_arrScene[static_cast<int>(m_eCurScene)] == nullptr)
		return;
	m_arrScene[static_cast<int>(m_eCurScene)]->Update();
	m_arrScene[static_cast<int>(m_eCurScene)]->LateUpdate();
}

void SceneManager::Render(HDC _memDC)
{
	if (m_eCurScene == SCENE_TYPE::END || m_arrScene[static_cast<int>(m_eCurScene)] == nullptr)
		return;
	m_arrScene[static_cast<int>(m_eCurScene)]->Render(_memDC);
}

void SceneManager::SceneChange(SCENE_TYPE _eSceneType)
{
	if (m_eCurScene != SCENE_TYPE::END && m_arrScene[static_cast<int>(m_eCurScene)] != nullptr)
		m_arrScene[static_cast<int>(m_eCurScene)]->Release();
	m_eCurScene = _eSceneType;
	m_arrScene[static_cast<int>(m_eCurScene)]->Init();
	Core::GetInstance()->CreateBackDC();
}
