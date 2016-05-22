#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "Engine.h"
#include <time.h>
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	srand(time(0));
	Engine* engine = new Engine();


	engine->LaunchLoop();

	return 0;
}