#pragma once

#include "ResourceManager.h"
#include"Block.h"


class Scene;
enum BOARD_START
{
	X = 480,
	Y = 65,
};

enum GAME_STATE
{
	NORMAL,
	SUCCESS,
	FAIL,
	DESTROYING,
	SETIING,
	CHECKING,
	WAITING,
	CLEARING,
};

enum
{
	ALL_COLORREMOVE_BLOCKTYPE = 1,
	ALL_LINEREMOVE_BLOCKTYPE = 3,
	ALL_NORMAL_BLOCKTYPE = 5,
	ALL_BLOCKTYPE = 9,
};
struct GameGoal
{
	Texture* m_pTexture;
	int m_iTaretScore;
	RECT m_Rect;

	GameGoal() {}
	GameGoal(Texture* _texture, int _TargetScore, RECT _rect)
	{
		m_pTexture = _texture;
		m_iTaretScore = _TargetScore;
		m_Rect = _rect;
	}
	void HitTarget()
	{
		if (m_iTaretScore > 0)
			--m_iTaretScore;
	}

};
class Board : public Object
{
private:
	Vector2 m_vecStartPosition;

	Texture* m_pScoreBoardTexture;
	std::vector<GameGoal> m_vecScoreBoard;
	int m_iChance;
	int m_iHeight;
	int m_iWidth;

	Texture* m_pBackGround;
	Vector2 m_vec2BackGroundPosition;

	GAME_STATE m_eGameState;

	std::vector<int> m_arrColumnCount;
	std::stack<Block*> m_sRemoveBlock; // 매치가 성공된 블럭들 (후보블럭들을 출전시키고 후보블럭들 벡터로 편입)
	std::vector<Block*> m_vecSubBlock; // 후보블럭들
	
	std::vector<std::vector<Block*>> m_Board;
	std::vector<int> m_vecStartPointY;
	std::vector<int> m_vecEndPointY;

	bool m_bMatch_5;
	bool m_bMatchCol_4;
	bool m_bMatchRow_4;
	bool m_bMatchCross;

	bool m_bSwitching;

	Block* m_pFirstBlock;
	Block* m_pSecondBlock;

	float m_fAccTime;
	float m_fSpeed;
public:
	Board();
	~Board();
	Vector2 GetStartPosition() { return m_vecStartPosition; }
	virtual void Update();
	virtual void Render(HDC _memDC);
	
	void SuccessProcess();
	void FailProcess();
	bool DestroyingProcess();
	void SettingProcess();
	void WaitingProcess();
	bool CheckingProcess();
	bool SwitchingProcess();
	bool IsReadyToUpdate();

	void RenderScoreBoard(HDC _memDC);
	virtual void Init(TEXTURE_TYPE _BoardType, std::vector<std::vector<int>> _boardInfo, int _chance, std::vector<GameGoal> _vecScoreBoard);
	
	void BoardReset();
	Texture* GetTexture() { return m_pBackGround; }
	void SetState(GAME_STATE _state) { m_eGameState = _state; }

	TEXTURE_TYPE GetRandomType(int _x, int _y);

	void CancleSelectedBlocks(Block* _block);
	void SelectBlock(Block* _block);
	void PositionSwitch();
	void ClearSelectedBlock();
	bool isNeighbor(Block* _FirstBlock, Block* _SecondBlock);
	bool CheckBoard();
	bool CanMatch();
	bool CheckMatch(Vector2 _position, TEXTURE_TYPE _type);

	bool isRemoveSpecial(Block* _block);
	bool RowLineCheck(Vector2 _position, TEXTURE_TYPE _type);
	bool ColLineCheck(Vector2 _position, TEXTURE_TYPE _type);

	bool WinCheck();
	bool isSuccess();
	bool BlockCheck(Block* _block);

	bool CheckBlock(Vector2 _position, TEXTURE_TYPE _type);
	bool CheckBoom();
	void BoomBlock();
	void RemoveBlock();
	void SetNewBlock();
	int FindNewPositionY(int _x, int _y);

	int GetNextY(Vector2 _position);

	void InitAllBoolean(){ m_bMatch_5 = false;m_bMatchCol_4 = false;m_bMatchRow_4 = false;m_bMatchCross = false; }
	void ClearArray() { for (int i = 0; i < m_iWidth; ++i)m_arrColumnCount[i] = 0; }
	void ClearStack() { while (!m_sRemoveBlock.empty()) { m_sRemoveBlock.pop(); } }

	void RemoveRow(Vector2 _position);
	void RemoveColumn(Vector2 _position);
	void RemoveCross(Vector2 _position);
	void RemoveAllSame(Block* _block);
	bool isOtherNormal();
	Vector2 GetNextDirection(DIRECTION _edirection);
};

