#pragma once

struct Vector2
{

	int m_ix;
	int m_iy;

	

	Vector2()
	{
		m_ix = 0  ;
		m_iy = 0;
		
	}
	Vector2(int _x, int _y)
	{
		m_ix = _x;
		m_iy = _y;
	}

	

	void operator += (Vector2 _vec2)
	{
		m_ix += _vec2.m_ix;
		m_iy += _vec2.m_iy;
	}
	void operator -= (Vector2 _vec2)
	{
		m_ix -= _vec2.m_ix;
		m_iy -= _vec2.m_iy;
	}

	bool operator == (Vector2 _vec2)
	{
		return (m_ix == _vec2.m_ix && m_iy == _vec2.m_iy);
	}
	Vector2 operator - (Vector2 _vec2)
	{
		Vector2 value;
		value.m_ix = m_ix - _vec2.m_ix;
		value.m_iy = m_iy - _vec2.m_iy;
		return value;
	}
	Vector2 operator + (Vector2 _vec2)
	{
		Vector2 value;
		value.m_ix = m_ix + _vec2.m_ix;
		value.m_iy = m_iy + _vec2.m_iy;
		return value;
	}

	
	
};

struct RenderPosition
{
	float m_fx;
	float m_fy;

	RenderPosition() { m_fx = 0.0f; m_fy = 0.0f; }
	RenderPosition(float _x, float _y)
	{
		m_fx = _x;
		m_fy = _y;
	}
	RenderPosition operator - (RenderPosition _vec2)
	{
		RenderPosition value;
		value.m_fx = m_fx - _vec2.m_fx;
		value.m_fy = m_fy - _vec2.m_fy;
		return value;
	}
	RenderPosition operator + (RenderPosition _vec2)
	{
		RenderPosition value;
		value.m_fx = m_fx + _vec2.m_fx;
		value.m_fy = m_fy + _vec2.m_fy;
		return value;
	}
	void operator += (RenderPosition _vec2)
	{
		m_fx += _vec2.m_fx;
		m_fy += _vec2.m_fy;
	}
	void operator -= (RenderPosition _vec2)
	{
		m_fx -= _vec2.m_fx;
		m_fy -= _vec2.m_fy;
	}
};