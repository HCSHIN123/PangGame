#pragma once
class TimerManager
{
	SINGLETON(TimerManager)
private:
	LARGE_INTEGER m_llPrevCount;//���� ����ó��Ƚ��
	LARGE_INTEGER m_llFrequency;//1�ʴ� ����ó��Ƚ��

	double m_dDeltaTime;// �������ӿ� �ɸ��� �ð� 
	double m_dAccTime;//�����ð�
	unsigned int m_iCallCount; //������ Ƚ��
	unsigned int m_iFPS;//������
public:
	void Init();
	void Update();
	double GetdDeltaTime() { return m_dDeltaTime; }
	float GetfDeltaTime() { return  m_dDeltaTime; }
	unsigned intGetFPS() { return m_iFPS; }

};

