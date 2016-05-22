#pragma once

class GameState
{
public:
	GameState();
	~GameState();

	virtual void Init(const char* args = nullptr) = 0;
	virtual void Update(const float dt) = 0;

private:

};

