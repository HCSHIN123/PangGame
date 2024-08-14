#pragma once
class Texture;

struct AnimNode
{
	Texture* m_pTexture;
	std::function<void()> m_callBack;
};

enum class ANIMATION_TYPE
{
	LOOP,
	ONCE,
	END,
};

class Animation
{
private:
	std::vector<AnimNode>	m_vecList;
	ANIMATION_TYPE			m_eType;
	float					m_fSpeed;
	float					m_fAccTime;
	int						m_iCurIndex;
public:
	Animation();
	~Animation();
	void Reset();
	void Init(std::vector<AnimNode> _vecList, ANIMATION_TYPE _eType, float _fSpeed);
	void Update();
	void Render(HDC _memDC, Vector2 _vec2Position);
	bool GetLive()
	{
		if (m_eType == ANIMATION_TYPE::LOOP)
			return true;
		else if (m_eType == ANIMATION_TYPE::ONCE && m_iCurIndex < m_vecList.size() - 1)
			return true;
		return false;
	}
};

