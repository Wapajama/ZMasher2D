#pragma once
#include "GameState.h"


class GamePlayState :
	public GameState
{
public:
	GamePlayState();
	~GamePlayState();

	void Init(const char* args = nullptr)override;
	void Update(const float dt)override;


private:
};

