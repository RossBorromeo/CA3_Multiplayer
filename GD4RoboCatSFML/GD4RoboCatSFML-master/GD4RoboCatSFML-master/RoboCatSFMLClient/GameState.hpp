#pragma once
#include "State.hpp"

class GameState : public State
{
public:
	GameState(StateStack& stack);

	virtual void HandleInput(const sf::Event& event) override;
	virtual void Update(float dt) override;
	virtual void Render(sf::RenderWindow& window) override;

private:
	bool m_networkInitialised = false;
};