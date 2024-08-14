#pragma once


class Object;
class Scene
{
protected:
	Vector2					m_vec2WindowStartPosition;
	Vector2					m_vec2WindowCenterPosition;
	Vector2					m_vec2WindowSize;
	std::vector<Object*>	m_arrObjects[static_cast<int>(OBJECT_GROUP::END)];
	std::string				m_strName;
public:
	Scene(std::string _strName);
	virtual ~Scene();
	virtual void Update();
	void LateUpdate();
	virtual void Render(HDC _memDC);
	virtual void Init() = 0;
	virtual void Release();
	const std::vector<Object*>& GetObjectGroup(OBJECT_GROUP _eObjectGrup) { return m_arrObjects[static_cast<int>(_eObjectGrup)]; }
	void SetWindowSize(int _iWidth, int _iHeight);
	void AddObject(Object* _object, OBJECT_GROUP _eGroup);
	
	Vector2 GetWindowSize() { return m_vec2WindowSize; }
	void SetName(const std::string _strName) { m_strName = _strName; }
};

