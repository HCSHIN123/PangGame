#pragma once
#include"Scene.h"
#include<fstream>
#include"Button.h"
class MapTool : public Scene
{
private:
	int m_iMapHeight;
	int m_iMapWidth;
	std::vector<std::vector<Button>> m_vecBlocks;
	int m_iChance;
	std::vector<int> m_vecTargetCount;

	std::string m_strCurrentFileName;
	std::ifstream m_Load;
	std::ofstream m_Save;
	Texture* m_pBackGround;
	Texture* m_pCurrentBlockTexture;
	TEXTURE_TYPE m_eCurrentType;
	std::vector<Button*> m_vecStageBtns;
	std::vector<Button*> m_vecBlockSelection;
	Button m_SaveButton;
public:
	// Scene을(를) 통해 상속됨
	void Init() override;
	void Update() override;
	void MouseOnCheck(POINT _pt, bool _isClick);
	void Render(HDC _memDC) override;
	void LoadStageInfo(std::string _stageName);
	void SaveStageInfo();
	MapTool(std::string _strName);
	~MapTool();
};

