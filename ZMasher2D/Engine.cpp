#include "Engine.h"
#include <SFML/Window/Keyboard.hpp>
#include "GamePlayState.h"
#include <stdlib.h>
#include "Time\TimerManager.h"
#include <assert.h>

__forceinline float randf(const float lo,const float hi)
{
	return (lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo))));
}

Engine::Engine() :m_Ticker(60)
{
	m_Shape.setRadius(1.f);
	m_Shape.setFillColor(sf::Color::Red);
	for (int i = 0; i < MAX_COMPONENTS; ++i)
	{
		m_TransComps[i].x = (rand() % WINDOW_WIDTH);
		m_TransComps[i].y = (rand() % WINDOW_HEIGHT);

		m_TransComps[i].m_Vel.x = float(static_cast<float>(rand() % 1000)-500.f)/1000.f;
		m_TransComps[i].m_Vel.y = float(static_cast<float>(rand() % 1000)-500.f)/1000.f;
		ZMasher::Normalize(m_TransComps[i].m_Vel);

	}
	for (int i = 0; i < MAX_COMPONENTS; ++i)
	{
		m_ShapeComp[i].m_Shape = &m_Shape;
	}
}


Engine::~Engine()
{
}

void Engine::LaunchLoop()
{
	m_GameState= new GamePlayState();
	m_GameState->Init();

	sf::RenderWindow window(sf::VideoMode(960, 720), "SFML works!");
	sf::CircleShape shape(20.f);
	shape.setFillColor(sf::Color::Green);

	static float x = 0.f;
	static float y = 0.f;
	const float speed = 300.f;

	while (window.isOpen())
	{
		TimerManager::GetInstance()->Update();
		const float realFrameTime = TimerManager::GetInstance()->GetMainTimer().TimeSinceLastFrame().GetSeconds();
		//if (m_Ticker.Update(realFrameTime))
		{
			const float frame_time = m_Ticker.GetFrameTime();
			if (WindowEvent(&window) == false)return;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) == true)
			{
				y -= speed * frame_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) == true)
			{
				x -= speed * frame_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) == true)
			{
				y += speed * frame_time;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) == true)
			{
				x += speed * frame_time;
			}
			shape.setPosition(x, y);

			//m_MoveManager.ProcessAgents(m_TransComps);

			//if (m_Ticker.NeedsMoreTime())
			{
				window.clear();
				window.draw(shape);
				m_RenderManager.RenderShapes(window, m_TransComps, m_Shape);
				window.display();
			}
		}
		//else
		//{
		//	Sleep(1);
		//}
	}

}

bool Engine::WindowEvent(sf::RenderWindow* window)
{
	sf::Event event;
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			return false;
			break;
		case sf::Event::Resized:
			window->setSize(window->getSize());
			break;
		default:
			break;
		}
	}
	return true;
}

inline void EdgeWarp(ZMasher::Vector2<float> &pos)
{
	if (pos.x < 0)
	{
		pos.x = WINDOW_WIDTH;
		return;
	}
	if (pos.x > WINDOW_WIDTH)
	{
		pos.x = 0;
		return;
	}
	if (pos.y < 0)
	{		
		pos.y = WINDOW_HEIGHT;
		return;
	}		
	if (pos.y > WINDOW_HEIGHT)
	{		
		pos.y = 0;
		return;
	}
}

void RandomMovementManager::ProcessAgents(TransformComp* transforms)
{
	const ZMasher::Vector2<float> middle(WINDOW_WIDTH*0.5f, WINDOW_HEIGHT*0.5f);
	//hardcoded to 1024
	for (int i = 0; i < MAX_COMPONENTS; ++i)
	{
		ProcessAgent(transforms[i]);
	}
}
#include <dvec.h>
union TEST_UN
{
	TEST_UN(const __m128& data)
	{
		m_Data = data;
	}
	TEST_UN(const ZMasher::Vector2<float> pos1, const ZMasher::Vector2<float> pos2)
	{
		m_Pos1 = pos1;
		m_Pos2 = pos2;
	}
	~TEST_UN() {}

	struct
	{
		ZMasher::Vector2<float> m_Pos1;
		ZMasher::Vector2<float> m_Pos2;
	};
	__m128 m_Data;
};

inline void RandomMovementManager::ProcessAgent(TransformComp& transform)
{
	//TEST_UN result(
	
	transform.m_Pos += transform.m_Vel;
	EdgeWarp(transform.m_Pos);

	//ZMasher::Vector2<float> middle_dir = ZMasher::GetNormalized(transforms[i].m_Pos - middle);
	//ZMasher::Vector2<float> random_push(randf(-1.f, 1.f), randf(-1.f, 1.f));
	//ZMasher::Normalize(random_push);
	//random_push *= 0.01f;
	//middle_dir += random_push;
	//transforms[i].m_Vel += middle_dir;
	//ZMasher::Normalize(transforms[i].m_Vel);
}

inline void RandomMovementManager::SIMDProcessAgent(TransformComp& transform1, TransformComp& transform2)
{
	__m128 temp[2];
	temp[0].m128_f32[0] = transform1.m_Pos.x;
	temp[0].m128_f32[1] = transform1.m_Pos.y;
	temp[0].m128_f32[2] = transform2.m_Pos.x;
	temp[0].m128_f32[3] = transform2.m_Pos.y;

	temp[1].m128_f32[0] = transform1.m_Vel.x;
	temp[1].m128_f32[1] = transform1.m_Vel.y;
	temp[1].m128_f32[2] = transform2.m_Vel.x;
	temp[1].m128_f32[3] = transform2.m_Vel.y;

	TEST_UN test(_mm_add_ps(temp[0], temp[1]));

	EdgeWarp(test.m_Pos1);
	EdgeWarp(test.m_Pos2);
	transform1.m_Pos = test.m_Pos1;
	transform2.m_Pos = test.m_Pos2;
}

void RenderManager::RenderShapes(sf::RenderWindow& window, TransformComp* transforms, sf::CircleShape& shape)
{
	//for (int i = 0; i < MAX_COMPONENTS; ++i)
	//{
	//	m_MoveManager.ProcessAgent(transforms[i]);
	//	shape.setPosition(transforms[i].x, transforms[i].y);
	//	if (i % 100 == 0)
	//	{
	//		window.draw(shape);
	//	}
	//}
	//int current_index = 0;
	//for (int i = 0; i < 512; ++i, ++current_index)
	//{
	//	for (int j = 0; j < 128; ++j, ++current_index)
	//	{
	//		const int index = current_index;
	//		m_MoveManager.ProcessAgent(transforms[index]);
	//		shape.setPosition(transforms[index].x, transforms[index].y);
	//		if (index%100 == 0)
	//		{
	//			window.draw(shape);
	//		}
	//	}
	//}

	int current_index = 0;
	const int cache = 4;
	//for (int i = 0; i < MAX_COMPONENTS; ++i, ++current_index)
	for (current_index; current_index < MAX_COMPONENTS/ cache; current_index+=2)
	{
		const int index = current_index;
		m_MoveManager.SIMDProcessAgent(transforms[index], transforms[index + 1]);
		shape.setPosition(transforms[index].x, transforms[index].y);
		if (index % 50 == 0)
		{
			window.draw(shape);
		}
	}
}