#pragma once

#define SINGLETON(Type) public:\
										static Type* GetInstance()\
										{\
											static  Type instance;\
											return &instance;\
										}\
									private:\
										Type();\
										~Type();

enum class OBJECT_GROUP
{
	BUTTON,
	BOARD,
	BLOCK,
	WALL_BLOCK,
	END,
	
};


enum DIRECTION
{
	START,
	LEFT = START,
	RIGHT,
	UP,
	DOWN,
	END,
	COLUMN_DIRECTION,
	ROW_DIRECTION,
};
static DIRECTION directions[4] = { UP, DOWN, LEFT, RIGHT };
enum
{
	MAX_LEVEL = 3,
};
enum BUTTON_POSITION
{
	START_X = 40,
	START_Y = 50,
	DISTANCE_Y = 150,
};