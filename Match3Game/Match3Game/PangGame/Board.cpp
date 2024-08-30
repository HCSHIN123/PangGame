#include "pch.h"
#include "Board.h"
#include "Texture.h"
#include "TimerManager.h"
#include "Scene.h"
#include"SceneManager.h"
#include "Core.h"
#include "User.h"
#include "BlockWall.h"


Board::Board()
{
	m_pBackGround = nullptr;
	m_pFirstBlock = nullptr;
	m_pSecondBlock = nullptr;
	m_eGameState = GAME_STATE::NORMAL;

	m_bMatch_5 = false;
	m_bMatchCol_4 = false;
	m_bMatchRow_4 = false;
	m_bMatchCross = false;
	m_bSwitching = false;
}

Board::~Board()
{
}

TEXTURE_TYPE Board::GetRandomType(int _x, int _y)
{

	int ColumnRandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;
	int RowRandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;

	if (_y < 2 && _x < 2) // ���� ���� ��� ���õ� ����� �ΰ� �����϶�
	{
		int RandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;
		return static_cast<TEXTURE_TYPE>(RandomBlock);
	}

	if (_y >= 2) // �Ʒ��� �ΰ��� ����� ������
	{
		if (m_Board[_y - 1][_x]->getBlockType() == m_Board[_y - 2][_x]->getBlockType())
			ColumnRandomBlock = m_Board[_y - 1][_x]->getBlockType();
	}
	if (_x >= 2) // ���ʿ� �ΰ��� ����� ������
	{
		if (m_Board[_y][_x - 1]->getBlockType() == m_Board[_y][_x - 2]->getBlockType())
			RowRandomBlock = m_Board[_y][_x - 1]->getBlockType();
	}

	if(typeid(m_Board[0][0]) == typeid(Block)){}

	int RandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;
	while (RandomBlock == ColumnRandomBlock || RandomBlock == RowRandomBlock)
	{
		RandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;
	}

	return static_cast<TEXTURE_TYPE>(RandomBlock);
}
void Board::Init(TEXTURE_TYPE _BoardType, std::vector<std::vector<int>> _boardInfo, int _chance, std::vector<GameGoal> _vecScoreBoard)
{
	srand(time(NULL));
	m_iHeight = _boardInfo.size();
	m_iWidth = _boardInfo[0].size();
	m_pBackGround = ResourceManager::GetInstance()->LoadTexture(_BoardType);
	m_iChance = _chance;
	m_vecScoreBoard = _vecScoreBoard;
	m_arrColumnCount.resize(m_iWidth);
	Scene* scene = SceneManager::GetInstance()->GetCurScene();
	assert(scene != nullptr);

	m_Board.resize(m_iHeight, std::vector<Block*>(m_iWidth, nullptr));

	for (int y = 0; y < m_iHeight; ++y) // ���弼��
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			int BlockType;
			Block* tmpBlock;

			if (_boardInfo[y][x] == 0)
			{
				tmpBlock = new Block();
				BlockType = GetRandomType(x, y);
				scene->Scene::AddObject(tmpBlock, OBJECT_GROUP::BLOCK);
				tmpBlock->Init({ x, y }, (TEXTURE_TYPE)BlockType, this, std::bind(&Board::BlockCheck, this, std::placeholders::_1));
			}
			else
			{
				tmpBlock = new BlockWall();
				BlockType = _boardInfo[y][x];
				scene->Scene::AddObject(tmpBlock, OBJECT_GROUP::WALL_BLOCK);
				if (BlockType == TEXTURE_TYPE::WALL)
				{
					tmpBlock->Init({ x, y }, (TEXTURE_TYPE)BlockType, this, std::bind(&Board::BlockCheck, this, std::placeholders::_1));
				}
				else
				{
					tmpBlock->Init({ x, y }, (TEXTURE_TYPE)GetRandomType(x, y), this, std::bind(&Board::BlockCheck, this, std::placeholders::_1));
					dynamic_cast<BlockWall*>(tmpBlock)->InitWall((TEXTURE_TYPE)_boardInfo[y][x]);

					Block* extraBlock = new Block();
					extraBlock->Init({ 0,0 }, (TEXTURE_TYPE)GetRandomType(x, y), this, std::bind(&Board::BlockCheck, this, std::placeholders::_1));
					m_sExtraBlock.push(extraBlock);
					extraBlock->SetActivate(false);
					scene->Scene::AddObject(extraBlock, OBJECT_GROUP::BLOCK);
				}
			}
			m_Board[y][x] = tmpBlock;
		}
	}
}



void Board::BoardReset()
{
	for (int y = 0; y < m_iHeight; ++y)
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			if (m_Board[y][x]->getBlockType() == WALL)
				continue;
			m_Board[y][x]->SetTextureType(GetRandomType(x, y));
			m_Board[y][x]->SetNewPosition({ x, y });
			m_Board[y][x]->Block::SetPosition({ x, -2 });
		}
	}
}

// FSM����
void Board::Update()
{
	if (!IsReadyToUpdate()) // ������Ʈ�� �Ұ����� ���¶�� ����(���ҵ��� ó����)
		return;

	switch (m_eGameState)
	{
	case GAME_STATE::SUCCESS: // ��������
	{
		SuccessProcess();
		return;
	}
	case GAME_STATE::FAIL: // ���л���
	{
		FailProcess();
		break;
	}
	case GAME_STATE::DESTROYING: // �ı����� ����
	{
		if (DestroyingProcess())
			return;
		
		break;
	}
	case GAME_STATE::SETIING: // ����� �����ϴ� ����
	{
		SettingProcess();
		break;
	}
	case GAME_STATE::WAITING: // ��ϵ�� ��ٸ��� ����
	{
		WaitingProcess();
		return;
	}
	case GAME_STATE::CHECKING: // ���� ������ ���� üũ �� �̼ǿϷ� üũ�ϴ� ����
	{
		if (CheckingProcess()) // ���� ���Ῡ�� Ȯ��
		{
			SceneManager::GetInstance()->SceneChange(SCENE_TYPE::MAIN);
			return;
		}
		break;
	}
	}

	if (m_pFirstBlock != nullptr && m_pSecondBlock != nullptr) // ������ �ΰ��� ����� ���õ�����
	{
		PositionSwitch();
		if (!isSuccess())
			m_eGameState = GAME_STATE::FAIL;
		else
			m_eGameState = GAME_STATE::SUCCESS;
	}
}

void Board::Render(HDC _memDC)
{
	BitBlt(_memDC, 0, 0, m_pBackGround->GetWidth(), m_pBackGround->GetHeight(),
		m_pBackGround->GetDC(), 0, 0, SRCCOPY);

	RenderScoreBoard(_memDC);
}

void Board::SuccessProcess()
{
	--m_iChance;
	ClearSelectedBlock();
	BoomBlock();
	m_eGameState = GAME_STATE::DESTROYING;
}

void Board::FailProcess()
{
	PositionSwitch();
}

bool Board::DestroyingProcess()
{
	if (!CheckBoom()) // ������ ��ϵ��� ������ �ִϸ��̼� �ϷῩ�� üũ
		return true;

	RemoveBlock(); // �ִϸ��̼� �Ϸ������ ��� ����
	m_eGameState = GAME_STATE::SETIING; // ���º���
	return false;
}

void Board::SettingProcess()
{
	SetNewBlock();
	m_eGameState = GAME_STATE::WAITING;
}

void Board::WaitingProcess()
{
	bool bFinish = true;
	for (int y = 0; y < m_iHeight; ++y)
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			if (m_Board[y][x] == nullptr)
				assert(NULL);
			if (!m_Board[y][x]->DropPosition())
				bFinish = false;
		}
	}

	if (bFinish)
		m_eGameState = GAME_STATE::CHECKING;
}

bool Board::CheckingProcess()
{
	if (CheckBoard()) // ������� ���� �� ��Ī���� üũ
	{
		BoomBlock();
		m_eGameState = GAME_STATE::DESTROYING;
		return false;
	}

	bool bWin = WinCheck(); //�¸�üũ
	if (m_iChance <= 0 && bWin == false) // �й�
	{
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("����)�������� ����ȭ������ �̵��մϴ�"), _T("�̼� ����"), MB_ICONQUESTION | MB_OK) == IDOK)
			return true;
	}
	else if (bWin == true) // �¸�
	{
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("����)�������� ����ȭ������ �̵��մϴ�"), _T("�̼� ����!!!"), MB_ICONQUESTION | MB_OK) == IDOK)
		{
			User::GetInstance()->StageClear();
			return true;
		}
	}

	if (CanMatch()) // ��ġ������ �� ����üũ
		m_eGameState = GAME_STATE::NORMAL;
	else
	{
		m_eGameState = GAME_STATE::DESTROYING;
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("��ġ ������ ����� ��� ���ġ�մϴ�"), _T("�˸�"), MB_ICONQUESTION | MB_OK) == IDOK)
			BoardReset();
	}

	return false;
}

bool Board::SwitchingProcess()
{
	if (m_pFirstBlock == nullptr || m_pSecondBlock == nullptr)
		return true;
	bool firstEnd = m_pFirstBlock->MovePosition();
	bool secondEnd = m_pSecondBlock->MovePosition();
	
	return firstEnd && secondEnd;
}

bool Board::IsReadyToUpdate()
{
	if (m_bSwitching)
	{
		if (!SwitchingProcess())
			return false;;

		m_bSwitching = false;
		if (m_eGameState == GAME_STATE::FAIL)
		{
			PositionSwitch();
			m_eGameState = GAME_STATE::NORMAL;
			return false;;
		}

		ClearSelectedBlock();
	}
	
	return true;
}

void Board::RenderScoreBoard(HDC _memDC)
{

	for (int i = 0; i < m_vecScoreBoard.size(); ++i)
	{
		GameGoal curBlock = m_vecScoreBoard[i];
		std::string TargetScore = "��ǥ : " + std::to_string(curBlock.m_iTaretScore);
		
		TransparentBlt(_memDC, curBlock.m_Rect.left, curBlock.m_Rect.top - ((curBlock.m_Rect.bottom - curBlock.m_Rect.top) / 2),
			curBlock.m_pTexture->GetWidth(), curBlock.m_pTexture->GetHeight(), curBlock.m_pTexture->GetDC(),
			0, 0, curBlock.m_pTexture->GetWidth(), curBlock.m_pTexture->GetHeight(), RGB(255, 0, 255));
		TextOut(_memDC, curBlock.m_Rect.right + 3, curBlock.m_Rect.top, TargetScore.c_str(), TargetScore.length());
	}

	std::string strChance = "������ȸ : " + std::to_string(m_iChance);
	TextOut(_memDC, m_vecScoreBoard.back().m_Rect.right - 50, m_vecScoreBoard.back().m_Rect.bottom - 15, strChance.c_str(), strChance.length());
}

void Board::CancleSelectedBlocks(Block* _block)
{
	if (_block == nullptr || m_pFirstBlock == nullptr)
		return;

	m_pFirstBlock->Cancle();
	m_pFirstBlock = nullptr;
	_block->Cancle();
}

void Board::SelectBlock(Block* _block)
{
	if (m_pFirstBlock == nullptr)
		m_pFirstBlock = _block;
	else
	{
		if ((int)_block->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE && _block->getBlockType() == m_pFirstBlock->getBlockType()) // �Ϲݺ���϶� ���� ������ ��� ���úҰ� �Ѵ� ��������
		{
			CancleSelectedBlocks(_block);
			return;
		}

		if (isNeighbor(m_pFirstBlock, _block) == true) // �̿��� ��ϳ����� ������ȿ
			m_pSecondBlock = _block;
		else										  // �ƴϸ� ���
		{
			CancleSelectedBlocks(_block);
		}
	}
}

void Board::PositionSwitch()
{
	if (m_pFirstBlock == nullptr || m_pSecondBlock == nullptr)
		return;

	Vector2 tmpPos = m_pFirstBlock->GetBoardPosition();

	m_pFirstBlock->Block::SetPosition(m_pSecondBlock->GetBoardPosition());
	m_pSecondBlock->Block::SetPosition(tmpPos);

	m_pFirstBlock->SetRenderPosition({ (float)tmpPos.m_ix, (float)tmpPos.m_iy });
	m_pSecondBlock->SetRenderPosition({ (float)m_pFirstBlock->GetBoardPosition().m_ix, (float)m_pFirstBlock->GetBoardPosition().m_iy });

	m_Board[m_pFirstBlock->GetBoardPosition().m_iy][m_pFirstBlock->GetBoardPosition().m_ix] = m_pFirstBlock;
	m_pFirstBlock->SetNewPosition( m_pFirstBlock->GetBoardPosition());

	m_Board[m_pSecondBlock->GetBoardPosition().m_iy][m_pSecondBlock->GetBoardPosition().m_ix] = m_pSecondBlock;
	m_pSecondBlock->SetNewPosition( m_pSecondBlock->GetBoardPosition());

	m_bSwitching = true;
}

void Board::ClearSelectedBlock()
{
	if (m_pFirstBlock == nullptr || m_pSecondBlock == nullptr)
		return;

	m_pFirstBlock->Cancle();
	m_pFirstBlock = nullptr;
	m_pSecondBlock->Cancle();
	m_pSecondBlock = nullptr;
	
}

bool Board::isNeighbor(Block* _FirstBlock, Block* _SecondBlock)
{

	Vector2 FirstPosition = _FirstBlock->GetBoardPosition();
	Vector2 SecondPosition = _SecondBlock->GetBoardPosition();

	for (DIRECTION dir : directions)
	{
		if (SecondPosition + GetNextDirection(dir) == FirstPosition)
			return true;
	}

	return false;
}

bool Board::CheckBoard()
{
	// ����ȭ 
	// ��ü��ϰ˻� -> �̵��� ��ϵ� �߽����� �˻�

	bool bmatch = false;

	while (!m_sMovedBlockPos.empty()) // ��ġ�� ����� ��ϵ��� ������θ� �˻�
	{
		Vector2 checkPos = m_sMovedBlockPos.top();	
		m_sMovedBlockPos.pop();

		if (m_Board[checkPos.m_iy][checkPos.m_ix] == nullptr || m_Board[checkPos.m_iy][checkPos.m_ix]->getBlockType() == WALL)
			continue;
		if (CheckBlock({ checkPos.m_ix,checkPos.m_iy }, m_Board[checkPos.m_iy][checkPos.m_ix]->getBlockType())) //��ġ�� �� �˻�, ó��
		{
			if (!isRemoveSpecial(m_Board[checkPos.m_iy][checkPos.m_ix]))
			{
				m_sRemoveBlock.push(m_Board[checkPos.m_iy][checkPos.m_ix]);
				m_Board[checkPos.m_iy][checkPos.m_ix] = nullptr;
			}
			bmatch = true;
		}
	}

	return bmatch;
}



bool Board::CanMatch()
{
	// ������ ��ϰ� �����¿��� �ϳ��� ���ٸ� �� ���� �������� ���ɼ� �˻�
	Vector2 Directions[4] = { GetNextDirection(DIRECTION::LEFT), GetNextDirection(DIRECTION::RIGHT), 
								GetNextDirection(DIRECTION::UP), GetNextDirection(DIRECTION::DOWN) };

	for (int y = 0; y < m_iHeight; ++y)
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			TEXTURE_TYPE CurType = m_Board[y][x]->getBlockType();
			if ((int)CurType >= (int)ALL_NORMAL_BLOCKTYPE && CurType != WALL)	// Ư������� �����Ѵٸ� �ٷ� True��ȯ
				return true;
			if (CurType == WALL)												// ���� �˻����� �ʰ� continue
				continue;

			for (Vector2 next : Directions)
			{
				int next2X = x + next.m_ix * 2;	// 2ĭ �� x��ǥ
				int next2Y = y + next.m_iy * 2; // 2ĭ �� y��ǥ

				// ���� 2ĭ �ڰ� �迭������ �ʰ��ϰų� ���̶�� continue
				if (next2X < 0 || next2X >= m_iWidth || next2Y < 0 || next2Y >= m_iHeight || m_Board[next2Y][next2X]->getBlockType() == WALL)
					continue;
				// ��ġ���ɼ� �˻�
				if (IsPossibleForMatch({ x,y }, next, CurType))
					return true;
			}
		}
	}
	return false;
}

bool Board::IsPossibleForMatch(Vector2 _pos, Vector2 _dir, TEXTURE_TYPE _type)
{
	Vector2 nextPos_1 = _pos + _dir;												// ����ġ + 1ĭ ����
	Vector2 nextPos_2 = { _pos.m_ix + _dir.m_ix * 2, _pos.m_iy + _dir.m_iy * 2 };	// ����ġ + 2ĭ ����				
	Vector2 Dirs[3] = { _dir, {_dir.m_iy, _dir.m_ix}, {-_dir.m_iy, -_dir.m_ix} };	// �˻��� �����(��������, ��������, ����������)

	// �̿��� ��ϵ��� �����Ҷ�	 ex)  X����X
	if ( _type == m_Board[nextPos_1.m_iy][nextPos_1.m_ix]->getBlockType()) 
	{
		for (Vector2 next : Dirs)	// ��������, ��������, ������ ��� �˻�
		{
			if (CheckMatch(nextPos_2 + next, _type))	// 2ĭ�ڸ� �������� �˻�
				return true;
		}
	}

	// �� ĭ �������ִ� ��ϵ��� �����Ҷ�  ex)  ��X��
	// ���̿� �ִ� ����� ���̸� �˻�X
	if (m_Board[nextPos_1.m_iy][nextPos_1.m_ix]->getBlockType() != WALL && _type == m_Board[nextPos_2.m_iy][nextPos_2.m_ix]->getBlockType())
	{
		for (Vector2 next : Dirs)	
		{
			if (next == _dir)		// ��������, �����⸸ �˻�
				continue;

			if (CheckMatch(nextPos_1 + next, _type))	// 1ĭ�ڸ� �������� �˻�
				return true;
		}
	}

	return false;
}

bool Board::CheckMatch(Vector2 _position, TEXTURE_TYPE _type)
{
	// �迭���� ����ó��
	if (_position.m_ix < 0 || _position.m_iy < 0 || _position.m_iy >= m_iHeight || _position.m_ix >= m_iWidth)
		return false;

	// �񱳰�� ��ȯ
	return (m_Board[_position.m_iy][_position.m_ix]->getBlockType() == _type);
}

// ��ġȮ�� ��, ������ ��ϵ� �� ���ǿ� �����ϸ� �ش����� Ư��������� ��ü
bool Board::isRemoveSpecial(Block* _block)	
{
	TEXTURE_TYPE type;	// ��ü�� Ư����� Ÿ��

	if (m_bMatch_5) // �켱���� �÷�->ũ�ν�->����(COL�� ROW�� ������ ��� ���� ���� ����)
		type = REMOVE_COLOR_BLOCK;
	else if (m_bMatchCross)
		type = REMOVE_CROSS_BLOCK;
	else if (m_bMatchCol_4)
		type = REMOVE_COL_BLOCK;
	else if (m_bMatchRow_4)
		type = REMOVE_ROW_BLOCK;
	else
		return false;

	m_vecScoreBoard[(int)_block->getBlockType()].HitTarget(); // �̼Ǻ������
	_block->SetTextureType(type);	// Ư��������� ��ü
	return true;
}

bool Board::RowLineCheck(Vector2 _position, TEXTURE_TYPE _type)
{
	int iCount = 0; // ���� Ÿ�� ����
	int Leftx, Rightx; // �¿� �������� ���� ���� x�ε���
	for (Leftx = _position.m_ix - 1; Leftx >= 0; --Leftx) // ���� Ÿ���� �����µ��� ���� Ž��
	{
		// �ٸ��� ���� Ż��
		if (m_Board[_position.m_iy][Leftx] == nullptr || m_Board[_position.m_iy][Leftx]->getBlockType() != _type)
			break;

		++iCount; // ī��Ʈ ����
	}

	for (Rightx = _position.m_ix + 1; Rightx < m_iWidth; ++Rightx) //������ Ž��
	{
		// �ٸ��� ���� Ż��
		if (m_Board[_position.m_iy][Rightx] == nullptr || m_Board[_position.m_iy][Rightx]->getBlockType() != _type)
			break;

		++iCount; // ī��Ʈ ����
	}

	if (iCount + 1 >= 3) // �������� ������ ���� Ÿ���� ������ 3�� �̻��̶�� �������Stack�� ����
	{
		for (int x = Leftx + 1; x < Rightx; ++x) 
		{
			if (m_Board[_position.m_iy][x] == m_Board[_position.m_iy][_position.m_ix]) // �������� �ǳʶڴ�
				continue;
			m_sRemoveBlock.push(m_Board[_position.m_iy][x]);
			m_Board[_position.m_iy][x] = nullptr;
		}

		if (iCount + 1 >= 5) // 5�� ��ġ�� �����ϸ� �÷���� ���� bool���� True
			m_bMatch_5 = true;
		else if (iCount + 1 >= 4) // 5���� �����ϰ� 4���� �����̶�� ���λ������ bool���� True
			m_bMatchRow_4 = true;
		// �� bool�������� ����Process���� Ư���� ������ ����
		return true; // 3��ġ ���� -> True��ȯ
	}
	else
		return false; // ���� -> False��ȯ
}

bool Board::ColLineCheck(Vector2 _position, TEXTURE_TYPE _type)
{
	int iCount = 0;
	int up_y, down_y;
	for (up_y = _position.m_iy - 1; up_y >= 0; --up_y)
	{
		
		if (m_Board[up_y][_position.m_ix] == nullptr || m_Board[up_y][_position.m_ix]->getBlockType() != _type)
			break;
		++iCount;
	}

	for (down_y = _position.m_iy + 1; down_y < m_iHeight; ++down_y)
	{
		
		if (m_Board[down_y][_position.m_ix] == nullptr || m_Board[down_y][_position.m_ix]->getBlockType() != _type)
			break;
		++iCount;
	}

	if (iCount + 1 >= 3)
	{
		for (int y = up_y + 1; y < down_y; ++y)
		{
			if (m_Board[y][_position.m_ix] == m_Board[_position.m_iy][_position.m_ix])
				continue;
			m_sRemoveBlock.push(m_Board[y][_position.m_ix]);
			m_Board[y][_position.m_ix] = nullptr;
		}

		if (iCount + 1 >= 5)
			m_bMatch_5 = true;
		else if (iCount + 1 >= 4)
			m_bMatchCol_4 = true;

		return true;
	}
	else
		return false;
}

bool Board::WinCheck()
{
	bool bAllClear = true;
	for (int i = 0; i < m_vecScoreBoard.size(); ++i)
	{
		if (m_vecScoreBoard[i].m_iTaretScore > 0)
			bAllClear = false;
	}

	return bAllClear;
}

bool Board::isSuccess()
{
	bool bSuccess = false;

	if (m_pFirstBlock->getBlockType() > REMOVE_COLOR_BLOCK)
	{
		Block* tmpblock = m_pFirstBlock;
		m_pFirstBlock = m_pSecondBlock;
		m_pSecondBlock = tmpblock;
	}
	if (m_pSecondBlock->getBlockType() == REMOVE_COLOR_BLOCK)
	{
		Block* tmpblock = m_pFirstBlock;
		m_pFirstBlock = m_pSecondBlock;
		m_pSecondBlock = tmpblock;
	}

	if (m_pFirstBlock->Check() == true)
		bSuccess = true;
	if (m_pSecondBlock->Check() == true)
		bSuccess = true;

	return bSuccess;
}

bool Board::BlockCheck(Block* _block)
{
	if (_block == nullptr)
		return false;

	bool bcheck = true;
	TEXTURE_TYPE type = _block->getBlockType();

	switch (type)
	{
	case REMOVE_CROSS_BLOCK:
	{
		RemoveCross(_block->GetBoardPosition());
		break;
	}
	case REMOVE_ROW_BLOCK:
	{
		RemoveRow(_block->GetBoardPosition());
		break;
	}
	case REMOVE_COL_BLOCK:
	{
		RemoveColumn(_block->GetBoardPosition());
		break;
	}
	case REMOVE_COLOR_BLOCK:
	{
		if (isOtherNormal() == false)
		{
			m_eGameState = FAIL;
			return false;
		}

		RemoveAllSame(_block);
		break;
	}
	default:
	{
		if (CheckBlock(_block->GetBoardPosition(), _block->getBlockType()) == true)
		{
			if (!isRemoveSpecial(_block))
			{
				m_sRemoveBlock.push(m_Board[_block->GetBoardPosition().m_iy][_block->GetBoardPosition().m_ix]);
				m_Board[_block->GetBoardPosition().m_iy][_block->GetBoardPosition().m_ix] = nullptr;
			}
			bcheck = true;
		}
		else
			bcheck = false;

		break;
	}
	}

	return bcheck;
}

bool Board::CheckBlock(Vector2 _position, TEXTURE_TYPE _type)
{
	bool isColMatched = false;
	bool isRowMatched = false;
	InitAllBoolean();

	isRowMatched = RowLineCheck(_position, _type);
	isColMatched = ColLineCheck(_position, _type);

	if (isColMatched && isRowMatched)
		m_bMatchCross = true;

	return (isColMatched || isRowMatched);
}


bool Board::CheckBoom()
{
	std::stack<Block*> BoomBlock = m_sRemoveBlock;

	while (!BoomBlock.empty())
	{
		if (BoomBlock.top()->isBoom())
			return false;

		BoomBlock.pop();
	}

	return true;
}

void Board::BoomBlock()
{
	std::stack<Block*> BoomBlock = m_sRemoveBlock;

	while (!BoomBlock.empty())
	{
		BoomBlock.top()->Boom();
		BoomBlock.pop();
	}
}

void Board::RemoveBlock()
{
	while (!m_sRemoveBlock.empty())
	{
		Block* pRemoveBlock = m_sRemoveBlock.top();		// ������Ͻ��ÿ��� �ϳ��� ����
		m_sRemoveBlock.pop();

		if (pRemoveBlock->GetBlockID() != BLOCK_ID::NORMAL_BLOCK)		// �Ϲݺ���� �ƴ� ���
		{
			TEXTURE_TYPE WallType = dynamic_cast<BlockWall*>(pRemoveBlock)->BlockWall::GetWallType();
			if (WallType == ICE_WALL)	// �������� ���
			{
				Block* newBlock = m_sExtraBlock.top();						// ���ο� ���� ������
				m_sExtraBlock.pop();					
				m_sBlockPool.push(newBlock);								// �������� ���� Pool�� ���ο� ���� �ִ´�
				++m_arrColumnCount[pRemoveBlock->GetBoardPosition().m_ix];	// ������ ����� �ش� ���� ����Count + 1
				pRemoveBlock->SetActivate(false);							// ����������� �Ϲݺ������ ��ü�����Ƿ� ������������ ��Ȱ��ȭ
			}
			else  // �������� ���
			{
				dynamic_cast<BlockWall*>(pRemoveBlock)->Reset((TEXTURE_TYPE)(rand() % ALL_NORMAL_BLOCKTYPE));	// ���� ���� ��ü
				m_Board[pRemoveBlock->GetBoardPosition().m_iy][pRemoveBlock->GetBoardPosition().m_ix] = pRemoveBlock;	// ��ġ�� ����
			}
		}
		else			 //�Ϲݺ���� ���
		{
			++m_arrColumnCount[pRemoveBlock->GetBoardPosition().m_ix];	// ������ ����� �ش� ���� ����Count + 1
			m_sBlockPool.push(pRemoveBlock);							// �������� ���� Pool�� �ִ´�
		}

		if ((int)pRemoveBlock->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE)	// ������ ����� �̼Ǻ���� ��� ó��
			m_vecScoreBoard[(int)pRemoveBlock->getBlockType()].HitTarget();
	}
}

void Board::SetNewBlock()
{
	m_sMovedBlockPos = {};

	for (int x = 0; x < m_iWidth; ++x)
	{
		if (m_arrColumnCount[x] == 0)	// ������ ���� ���� ���̸� �н�(����ȭ)
			continue;
		--m_arrColumnCount[x];

		int newY = m_iHeight - 1;	// ���ο� y������

		// �����ִ� ��(�ı����� ���� ��)�� ��ġ����
		for (int y = m_iHeight - 1; y >= 0; --y)	
		{
			if (m_Board[y][x] == nullptr || dynamic_cast<BlockWall*>(m_Board[y][x]) != nullptr) // �����ų� ���� ���� �н�
				continue;
			else
			{
				newY = FindNewPositionY(x, newY);				// ���� ������ (���� �ƴϸ鼭 �� ����)���� �Ʒ� y������ ���ο� ��ġ ����
				m_Board[newY][x] = m_Board[y][x];
				m_Board[newY][x]->SetNewPosition({ x, newY });	
				m_sMovedBlockPos.push({ x, newY });				// ������ ��Ͻ��ÿ� �߰�(���� ����˻�ÿ� ��ġ�˻���)
				--newY;
			}
		}

		// ���ο� ���� ��ġ ����
		newY = FindNewPositionY(x, newY);
		if (newY < 0)				// newY < 0�̶�� ���翭���� ������ ���� ���°��̹Ƿ� �ѱ��
			continue;

		int newBlockCount = newY;	// ������������ġ�� �����ϱ� ���� ���翭�� ������ ������ ���� �����صд�
		while (newY >= 0)						
		{
			newY = FindNewPositionY(x, newY);
			if (newY < 0)
				break;;

			Block* pSubBlock;
			pSubBlock = m_sBlockPool.top();			// �����Ҷ� ������ �ٽ� ���� �������� ��������Ų��(������ƮǮ��)
			pSubBlock->Block::Reset({ x,0 }, (TEXTURE_TYPE)(rand() % ALL_NORMAL_BLOCKTYPE));
			pSubBlock->SetNewPosition({ x,newY });
			m_sMovedBlockPos.push({ x, newY });		// ������ ��Ͻ��ÿ� �߰�(���� ����˻�ÿ� ��ġ�˻���)
			m_Board[newY][x] = pSubBlock;
			m_Board[newY][x]->SetPosition({ x, newY - newBlockCount - 1 });	// ��������ġ = ���ο� ��ġ - ���� ������ �� �� - 1�� �Ͽ� �ڿ������� ���� ���ڸ��� �������� �Ѵ�
			pSubBlock->SetActivate(true);
			--newY;
			m_sBlockPool.pop();
		}
	}

}

int Board::FindNewPositionY(int _x, int _y)
{
	while (_y >= 0 && m_Board[_y][_x] != nullptr && dynamic_cast<BlockWall*>(m_Board[_y][_x]) != nullptr)
	{
		--_y;
	}
	return _y;
}

int Board::GetNextY(Vector2 _position)
{
	int finderY = _position.m_iy;
	while (m_Board[finderY][_position.m_ix] == nullptr && 0 < finderY)
	{
		--finderY;
	}
	if (finderY <= 0)
		return -1;

	return finderY;
}


void Board::RemoveRow(Vector2 _position)
{

	for (int x = 0; x < m_iWidth; ++x)
	{
		if (m_Board[_position.m_iy][x] == nullptr || m_Board[_position.m_iy][x]->getBlockType() == WALL)
			continue;

		TEXTURE_TYPE Curtype = m_Board[_position.m_iy][x]->getBlockType();
		if (Curtype == REMOVE_COL_BLOCK)
			RemoveColumn({ x, _position.m_iy });
		else if (Curtype == REMOVE_CROSS_BLOCK)
		{
			m_sRemoveBlock.push(m_Board[_position.m_iy][x]);
			m_Board[_position.m_iy][x] = nullptr;
			RemoveColumn({ x, _position.m_iy });
		}
		else
		{
			m_sRemoveBlock.push(m_Board[_position.m_iy][x]);
			m_Board[_position.m_iy][x] = nullptr;
		}

	}

	m_eGameState = DESTROYING;
}

void Board::RemoveColumn(Vector2 _position)
{

	for (int y = 0; y < m_iHeight; ++y)
	{
		if (m_Board[y][_position.m_ix] == nullptr || m_Board[y][_position.m_ix]->getBlockType() == WALL)
			continue;

		TEXTURE_TYPE Curtype = m_Board[y][_position.m_ix]->getBlockType();
		if (Curtype == REMOVE_ROW_BLOCK)
			RemoveRow({ _position.m_ix,y });
		else if (Curtype == REMOVE_CROSS_BLOCK)
		{
			m_sRemoveBlock.push(m_Board[y][_position.m_ix]);
			m_Board[y][_position.m_ix] = nullptr;
			RemoveRow({ _position.m_ix,y });
		}
		else
		{
			m_sRemoveBlock.push(m_Board[y][_position.m_ix]);
			m_Board[y][_position.m_ix] = nullptr;
		}
	}

	m_eGameState = DESTROYING;
}

void Board::RemoveCross(Vector2 _position)
{
	RemoveColumn(_position);
	RemoveRow(_position);

	m_eGameState = DESTROYING;
}

void Board::RemoveAllSame(Block* _block)
{
	Block* targetBlock = _block == m_pFirstBlock ? m_pSecondBlock : m_pFirstBlock;
	TEXTURE_TYPE type = targetBlock->getBlockType();

	if (type > REMOVE_COLOR_BLOCK)
		return;

	for (int y = 0; y < m_iHeight; ++y)
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			if (m_Board[y][x]->getBlockType() == type)
			{
				m_sRemoveBlock.push(m_Board[y][x]);
				m_Board[y][x] = nullptr;
			}
		}
	}
	m_sRemoveBlock.push(_block);
	m_Board[_block->GetBoardPosition().m_iy][_block->GetBoardPosition().m_ix] = nullptr;


	m_eGameState = DESTROYING;
}

bool Board::isOtherNormal()
{
	if (m_pFirstBlock == nullptr || m_pSecondBlock == nullptr)
		return false;

	if (m_pFirstBlock->getBlockType() == REMOVE_COLOR_BLOCK)
		return ((int)m_pSecondBlock->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE);
	else if (m_pSecondBlock->getBlockType() == REMOVE_COLOR_BLOCK)
		return((int)m_pFirstBlock->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE);

	return false;
}



Vector2 Board::GetNextDirection(DIRECTION _edirection)
{
	switch (_edirection)
	{
	case DIRECTION::LEFT:		return Vector2{ -1,0 };
	case DIRECTION::DOWN:	return Vector2{ 0,1 };
	case DIRECTION::RIGHT:		return Vector2{ 1,0 };
	case DIRECTION::UP:			return Vector2{ 0,-1 };
	default:							return Vector2{ 0,0 };
	}
}

