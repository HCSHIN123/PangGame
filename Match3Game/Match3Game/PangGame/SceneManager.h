#pragma once
#include "Scene.h"
enum class SCENE_TYPE
{
	MAIN,
	GAME_SCENE,
	MAP_TOOL,
	END,
};
class Scene;
class SceneManager
{
	SINGLETON(SceneManager)
private:
	Scene* m_arrScene[static_cast<int>(SCENE_TYPE::END)];
	SCENE_TYPE m_eCurScene;
public:
	void Init();
	void Update();
	void Render(HDC _memDC);

	void SceneChange(SCENE_TYPE _eSceneType);
	Scene* GetCurScene() {
		if (m_eCurScene == SCENE_TYPE::END)
			return nullptr;
		return m_arrScene[static_cast<int>(m_eCurScene)];
	}
};

