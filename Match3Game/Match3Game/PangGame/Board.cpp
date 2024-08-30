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

	if (_y < 2 && _x < 2) // 위쪽 왼쪽 모두 세팅된 블록이 두개 이하일때
	{
		int RandomBlock = rand() % ALL_NORMAL_BLOCKTYPE;
		return static_cast<TEXTURE_TYPE>(RandomBlock);
	}

	if (_y >= 2) // 아래에 두개의 블록이 있을때
	{
		if (m_Board[_y - 1][_x]->getBlockType() == m_Board[_y - 2][_x]->getBlockType())
			ColumnRandomBlock = m_Board[_y - 1][_x]->getBlockType();
	}
	if (_x >= 2) // 왼쪽에 두개의 블록이 있을때
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

	for (int y = 0; y < m_iHeight; ++y) // 보드세팅
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

// FSM패턴
void Board::Update()
{
	if (!IsReadyToUpdate()) // 업데이트가 불가능한 상태라면 리턴(스왑동작 처리중)
		return;

	switch (m_eGameState)
	{
	case GAME_STATE::SUCCESS: // 성공상태
	{
		SuccessProcess();
		return;
	}
	case GAME_STATE::FAIL: // 실패상태
	{
		FailProcess();
		break;
	}
	case GAME_STATE::DESTROYING: // 파괴중인 상태
	{
		if (DestroyingProcess())
			return;
		
		break;
	}
	case GAME_STATE::SETIING: // 새블록 세팅하는 상태
	{
		SettingProcess();
		break;
	}
	case GAME_STATE::WAITING: // 블록드랍 기다리는 상태
	{
		WaitingProcess();
		return;
	}
	case GAME_STATE::CHECKING: // 새로 스폰된 보드 체크 및 미션완료 체크하는 상태
	{
		if (CheckingProcess()) // 게임 종료여부 확인
		{
			SceneManager::GetInstance()->SceneChange(SCENE_TYPE::MAIN);
			return;
		}
		break;
	}
	}

	if (m_pFirstBlock != nullptr && m_pSecondBlock != nullptr) // 나란한 두개의 블록이 선택됐을때
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
	if (!CheckBoom()) // 삭제된 블록들의 터지는 애니메이션 완료여부 체크
		return true;

	RemoveBlock(); // 애니메이션 완료됐으면 블록 삭제
	m_eGameState = GAME_STATE::SETIING; // 상태변경
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
	if (CheckBoard()) // 재생성된 블럭들 중 매칭여부 체크
	{
		BoomBlock();
		m_eGameState = GAME_STATE::DESTROYING;
		return false;
	}

	bool bWin = WinCheck(); //승리체크
	if (m_iChance <= 0 && bWin == false) // 패배
	{
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("실패)스테이지 선택화면으로 이동합니다"), _T("미션 실패"), MB_ICONQUESTION | MB_OK) == IDOK)
			return true;
	}
	else if (bWin == true) // 승리
	{
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("성공)스테이지 선택화면으로 이동합니다"), _T("미션 성공!!!"), MB_ICONQUESTION | MB_OK) == IDOK)
		{
			User::GetInstance()->StageClear();
			return true;
		}
	}

	if (CanMatch()) // 매치가능한 블럭 유무체크
		m_eGameState = GAME_STATE::NORMAL;
	else
	{
		m_eGameState = GAME_STATE::DESTROYING;
		if (MessageBox(Core::GetInstance()->GethWnd(), _T("매치 가능한 블록이 없어서 재배치합니다"), _T("알림"), MB_ICONQUESTION | MB_OK) == IDOK)
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
		std::string TargetScore = "목표 : " + std::to_string(curBlock.m_iTaretScore);
		
		TransparentBlt(_memDC, curBlock.m_Rect.left, curBlock.m_Rect.top - ((curBlock.m_Rect.bottom - curBlock.m_Rect.top) / 2),
			curBlock.m_pTexture->GetWidth(), curBlock.m_pTexture->GetHeight(), curBlock.m_pTexture->GetDC(),
			0, 0, curBlock.m_pTexture->GetWidth(), curBlock.m_pTexture->GetHeight(), RGB(255, 0, 255));
		TextOut(_memDC, curBlock.m_Rect.right + 3, curBlock.m_Rect.top, TargetScore.c_str(), TargetScore.length());
	}

	std::string strChance = "남은기회 : " + std::to_string(m_iChance);
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
		if ((int)_block->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE && _block->getBlockType() == m_pFirstBlock->getBlockType()) // 일반블록일때 같은 종류의 블록 선택불가 둘다 선택해제
		{
			CancleSelectedBlocks(_block);
			return;
		}

		if (isNeighbor(m_pFirstBlock, _block) == true) // 이웃한 블록끼리만 선택유효
			m_pSecondBlock = _block;
		else										  // 아니면 취소
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
	// 최적화 
	// 전체블록검사 -> 이동한 블록들 중심으로 검사

	bool bmatch = false;

	while (!m_sMovedBlockPos.empty()) // 위치가 변경된 블록들을 대상으로만 검사
	{
		Vector2 checkPos = m_sMovedBlockPos.top();	
		m_sMovedBlockPos.pop();

		if (m_Board[checkPos.m_iy][checkPos.m_ix] == nullptr || m_Board[checkPos.m_iy][checkPos.m_ix]->getBlockType() == WALL)
			continue;
		if (CheckBlock({ checkPos.m_ix,checkPos.m_iy }, m_Board[checkPos.m_iy][checkPos.m_ix]->getBlockType())) //매치된 블럭 검사, 처리
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
	// 기준점 블록과 상하좌우중 하나라도 같다면 그 방향 기준으로 가능성 검사
	Vector2 Directions[4] = { GetNextDirection(DIRECTION::LEFT), GetNextDirection(DIRECTION::RIGHT), 
								GetNextDirection(DIRECTION::UP), GetNextDirection(DIRECTION::DOWN) };

	for (int y = 0; y < m_iHeight; ++y)
	{
		for (int x = 0; x < m_iWidth; ++x)
		{
			TEXTURE_TYPE CurType = m_Board[y][x]->getBlockType();
			if ((int)CurType >= (int)ALL_NORMAL_BLOCKTYPE && CurType != WALL)	// 특수블록이 존재한다면 바로 True반환
				return true;
			if (CurType == WALL)												// 벽은 검사하지 않고 continue
				continue;

			for (Vector2 next : Directions)
			{
				int next2X = x + next.m_ix * 2;	// 2칸 뒤 x좌표
				int next2Y = y + next.m_iy * 2; // 2칸 뒤 y좌표

				// 다음 2칸 뒤가 배열범위를 초과하거나 벽이라면 continue
				if (next2X < 0 || next2X >= m_iWidth || next2Y < 0 || next2Y >= m_iHeight || m_Board[next2Y][next2X]->getBlockType() == WALL)
					continue;
				// 매치가능성 검사
				if (IsPossibleForMatch({ x,y }, next, CurType))
					return true;
			}
		}
	}
	return false;
}

bool Board::IsPossibleForMatch(Vector2 _pos, Vector2 _dir, TEXTURE_TYPE _type)
{
	Vector2 nextPos_1 = _pos + _dir;												// 현위치 + 1칸 다음
	Vector2 nextPos_2 = { _pos.m_ix + _dir.m_ix * 2, _pos.m_iy + _dir.m_iy * 2 };	// 현위치 + 2칸 다음				
	Vector2 Dirs[3] = { _dir, {_dir.m_iy, _dir.m_ix}, {-_dir.m_iy, -_dir.m_ix} };	// 검사할 방향들(원래방향, 수직방향, 역수직방향)

	// 이웃한 블록들이 동일할때	 ex)  XㅁㅁX
	if ( _type == m_Board[nextPos_1.m_iy][nextPos_1.m_ix]->getBlockType()) 
	{
		for (Vector2 next : Dirs)	// 원래방향, 수직방향, 역방향 모두 검사
		{
			if (CheckMatch(nextPos_2 + next, _type))	// 2칸뒤를 기준으로 검사
				return true;
		}
	}

	// 한 칸 떨어져있는 블록들이 동일할때  ex)  ㅁXㅁ
	// 사이에 있는 블록이 벽이면 검사X
	if (m_Board[nextPos_1.m_iy][nextPos_1.m_ix]->getBlockType() != WALL && _type == m_Board[nextPos_2.m_iy][nextPos_2.m_ix]->getBlockType())
	{
		for (Vector2 next : Dirs)	
		{
			if (next == _dir)		// 수직방향, 역방향만 검사
				continue;

			if (CheckMatch(nextPos_1 + next, _type))	// 1칸뒤를 기준으로 검사
				return true;
		}
	}

	return false;
}

bool Board::CheckMatch(Vector2 _position, TEXTURE_TYPE _type)
{
	// 배열범위 예외처리
	if (_position.m_ix < 0 || _position.m_iy < 0 || _position.m_iy >= m_iHeight || _position.m_ix >= m_iWidth)
		return false;

	// 비교결과 반환
	return (m_Board[_position.m_iy][_position.m_ix]->getBlockType() == _type);
}

// 매치확인 후, 스왑한 블록들 중 조건에 부합하면 해당블록을 특수블록으로 교체
bool Board::isRemoveSpecial(Block* _block)	
{
	TEXTURE_TYPE type;	// 교체될 특수블록 타입

	if (m_bMatch_5) // 우선순위 컬러->크로스->방향(COL과 ROW는 어차피 모두 참일 수가 없음)
		type = REMOVE_COLOR_BLOCK;
	else if (m_bMatchCross)
		type = REMOVE_CROSS_BLOCK;
	else if (m_bMatchCol_4)
		type = REMOVE_COL_BLOCK;
	else if (m_bMatchRow_4)
		type = REMOVE_ROW_BLOCK;
	else
		return false;

	m_vecScoreBoard[(int)_block->getBlockType()].HitTarget(); // 미션블록적용
	_block->SetTextureType(type);	// 특수블록으로 교체
	return true;
}

bool Board::RowLineCheck(Vector2 _position, TEXTURE_TYPE _type)
{
	int iCount = 0; // 같은 타입 갯수
	int Leftx, Rightx; // 좌우 방향으로 가는 각각 x인덱스
	for (Leftx = _position.m_ix - 1; Leftx >= 0; --Leftx) // 같은 타입이 나오는동안 왼쪽 탐색
	{
		// 다르면 루프 탈출
		if (m_Board[_position.m_iy][Leftx] == nullptr || m_Board[_position.m_iy][Leftx]->getBlockType() != _type)
			break;

		++iCount; // 카운트 증가
	}

	for (Rightx = _position.m_ix + 1; Rightx < m_iWidth; ++Rightx) //오른쪽 탐색
	{
		// 다르면 루프 탈출
		if (m_Board[_position.m_iy][Rightx] == nullptr || m_Board[_position.m_iy][Rightx]->getBlockType() != _type)
			break;

		++iCount; // 카운트 증가
	}

	if (iCount + 1 >= 3) // 기준점을 포함해 같은 타입의 갯수가 3개 이상이라면 삭제대상Stack에 저장
	{
		for (int x = Leftx + 1; x < Rightx; ++x) 
		{
			if (m_Board[_position.m_iy][x] == m_Board[_position.m_iy][_position.m_ix]) // 기준점은 건너뛴다
				continue;
			m_sRemoveBlock.push(m_Board[_position.m_iy][x]);
			m_Board[_position.m_iy][x] = nullptr;
		}

		if (iCount + 1 >= 5) // 5개 매치에 성공하면 컬러블록 생성 bool변수 True
			m_bMatch_5 = true;
		else if (iCount + 1 >= 4) // 5개는 실패하고 4개는 성공이라면 가로삭제블록 bool변수 True
			m_bMatchRow_4 = true;
		// 저 bool변수들은 삭제Process에서 특수블럭 생성에 사용됨
		return true; // 3매치 성공 -> True반환
	}
	else
		return false; // 실패 -> False반환
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
		Block* pRemoveBlock = m_sRemoveBlock.top();		// 삭제블록스택에서 하나를 꺼냄
		m_sRemoveBlock.pop();

		if (pRemoveBlock->GetBlockID() != BLOCK_ID::NORMAL_BLOCK)		// 일반블록이 아닌 경우
		{
			TEXTURE_TYPE WallType = dynamic_cast<BlockWall*>(pRemoveBlock)->BlockWall::GetWallType();
			if (WallType == ICE_WALL)	// 얼음벽인 경우
			{
				Block* newBlock = m_sExtraBlock.top();						// 새로운 블럭을 꺼낸다
				m_sExtraBlock.pop();					
				m_sBlockPool.push(newBlock);								// 리스폰을 위해 Pool에 새로운 블럭을 넣는다
				++m_arrColumnCount[pRemoveBlock->GetBoardPosition().m_ix];	// 삭제된 블록의 해당 열에 삭제Count + 1
				pRemoveBlock->SetActivate(false);							// 얼음벽블록이 일반블록으로 대체됐으므로 기존얼음벽은 비활성화
			}
			else  // 감옥벽인 경우
			{
				dynamic_cast<BlockWall*>(pRemoveBlock)->Reset((TEXTURE_TYPE)(rand() % ALL_NORMAL_BLOCKTYPE));	// 내부 블럭만 교체
				m_Board[pRemoveBlock->GetBoardPosition().m_iy][pRemoveBlock->GetBoardPosition().m_ix] = pRemoveBlock;	// 위치도 유지
			}
		}
		else			 //일반블록인 경우
		{
			++m_arrColumnCount[pRemoveBlock->GetBoardPosition().m_ix];	// 삭제된 블록의 해당 열에 삭제Count + 1
			m_sBlockPool.push(pRemoveBlock);							// 리스폰을 위해 Pool에 넣는다
		}

		if ((int)pRemoveBlock->getBlockType() < (int)ALL_NORMAL_BLOCKTYPE)	// 삭제된 블록이 미션블록일 경우 처리
			m_vecScoreBoard[(int)pRemoveBlock->getBlockType()].HitTarget();
	}
}

void Board::SetNewBlock()
{
	m_sMovedBlockPos = {};

	for (int x = 0; x < m_iWidth; ++x)
	{
		if (m_arrColumnCount[x] == 0)	// 삭제된 블럭이 없는 열이면 패스(최적화)
			continue;
		--m_arrColumnCount[x];

		int newY = m_iHeight - 1;	// 새로운 y포지션

		// 남아있는 블럭(파괴되지 않은 블럭)들 위치세팅
		for (int y = m_iHeight - 1; y >= 0; --y)	
		{
			if (m_Board[y][x] == nullptr || dynamic_cast<BlockWall*>(m_Board[y][x]) != nullptr) // 터졌거나 벽인 블럭은 패스
				continue;
			else
			{
				newY = FindNewPositionY(x, newY);				// 현재 열에서 (벽이 아니면서 빈 곳인)가장 아래 y값으로 새로운 위치 세팅
				m_Board[newY][x] = m_Board[y][x];
				m_Board[newY][x]->SetNewPosition({ x, newY });	
				m_sMovedBlockPos.push({ x, newY });				// 움직인 블록스택에 추가(추후 보드검사시에 매치검사대상)
				--newY;
			}
		}

		// 새로운 블럭들 위치 세팅
		newY = FindNewPositionY(x, newY);
		if (newY < 0)				// newY < 0이라면 현재열에는 삭제된 블럭이 없는것이므로 넘긴다
			continue;

		int newBlockCount = newY;	// 리스폰시작위치를 지정하기 위해 현재열에 생성될 블럭들의 수를 저장해둔다
		while (newY >= 0)						
		{
			newY = FindNewPositionY(x, newY);
			if (newY < 0)
				break;;

			Block* pSubBlock;
			pSubBlock = m_sBlockPool.top();			// 삭제할때 넣은거 다시 빼서 랜덤으로 리스폰시킨다(오브젝트풀링)
			pSubBlock->Block::Reset({ x,0 }, (TEXTURE_TYPE)(rand() % ALL_NORMAL_BLOCKTYPE));
			pSubBlock->SetNewPosition({ x,newY });
			m_sMovedBlockPos.push({ x, newY });		// 움직인 블록스택에 추가(추후 보드검사시에 매치검사대상)
			m_Board[newY][x] = pSubBlock;
			m_Board[newY][x]->SetPosition({ x, newY - newBlockCount - 1 });	// 리스폰위치 = 새로운 위치 - 같이 생성될 블럭 수 - 1로 하여 자연스럽게 블럭이 제자리로 떨어지게 한다
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

