#include "pch.h"
#include "TimerManager.h"
#include "Core.h"


TimerManager::TimerManager()
{
	m_dAccTime = 0.0;
	m_dDeltaTime = 0.0;
	m_iCallCount = 0;
	m_iFPS = 0;
	m_llFrequency = {};
	m_llPrevCount = {};
}
TimerManager::~TimerManager()
{

}

void TimerManager::Init()
{
	m_iCallCount = 0;
	m_dAccTime = 0.0;
	//호출시점의 CPU 동작횟수
	QueryPerformanceCounter(&m_llPrevCount);
	//1초당 CPU 동작횟수
	QueryPerformanceFrequency(&m_llFrequency);
}

void TimerManager::Update()
{
	LARGE_INTEGER llCurCount;//현재 연산처리횟수
	QueryPerformanceCounter(&llCurCount);

	m_dDeltaTime = (double)(llCurCount.QuadPart - m_llPrevCount.QuadPart) / (double)m_llFrequency.QuadPart;
	m_llPrevCount = llCurCount;

	++m_iCallCount;
	m_dAccTime += m_dDeltaTime;
	if (m_dAccTime >= 1)
	{
		m_iFPS = m_iCallCount;
		m_dAccTime = 0.0;
		m_iCallCount = 0;
	}
}
