#pragma once
#include "Vector2.h"
#include <SFML/Graphics.hpp>
#include "Time\Ticker.h"

class GameState;
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
namespace sf
{
	class RenderWindow;
}

struct TransformComp
{
	union
	{
		ZMasher::Vector2<float> m_Pos;
		struct
		{
			float x;
			float y;
		};
	};

	ZMasher::Vector2<float> m_Vel;

	TransformComp() {};
	~TransformComp() {};
};

struct ShapeComp
{
	sf::CircleShape* m_Shape;
};

class RandomMovementManager
{
public:
	void ProcessAgents(TransformComp* transforms);
	inline void ProcessAgent(TransformComp& transform);
	inline void SIMDProcessAgent(TransformComp& transform1, TransformComp& transform2);
private:

};

class RenderManager
{
public:
	void RenderShapes(sf::RenderWindow& window, TransformComp* transforms, sf::CircleShape& shape);
private:
	RandomMovementManager m_MoveManager;
};

#define MAX_COMPONENTS 131072*8
class Engine
{
public:
	Engine();
	~Engine();

	void LaunchLoop();

private:

	bool WindowEvent(sf::RenderWindow* window);

	GameState* m_GameState;
	sf::CircleShape m_Shape;
	TransformComp m_TransComps[MAX_COMPONENTS];
	ShapeComp m_ShapeComp[MAX_COMPONENTS];

	RenderManager m_RenderManager;
	RandomMovementManager m_MoveManager;
	Ticker m_Ticker;

};
