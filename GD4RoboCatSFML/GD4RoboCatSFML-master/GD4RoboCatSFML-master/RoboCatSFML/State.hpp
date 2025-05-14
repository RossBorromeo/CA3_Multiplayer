#pragma once

#include <SFML/Graphics.hpp>

class StateStack; // Forward declaration

class State
{
public:
	explicit State(StateStack& stack) : mStack(stack) {}
	virtual ~State() = default;

	virtual void HandleInput(const sf::Event& event) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(sf::RenderWindow& window) = 0;

protected:
	StateStack& mStack;
};
