#pragma once
class TimerManager
{
	SINGLETON(TimerManager)
private:
	LARGE_INTEGER m_llPrevCount;//이전 연산처리횟수
	LARGE_INTEGER m_llFrequency;//1초당 연산처리횟수

	double m_dDeltaTime;// 한프레임에 걸리는 시간 
	double m_dAccTime;//누적시간
	unsigned int m_iCallCount; //프레임 횟수
	unsigned int m_iFPS;//프레임
public:
	void Init();
	void Update();
	double GetdDeltaTime() { return m_dDeltaTime; }
	float GetfDeltaTime() { return  m_dDeltaTime; }
	unsigned intGetFPS() { return m_iFPS; }

};

