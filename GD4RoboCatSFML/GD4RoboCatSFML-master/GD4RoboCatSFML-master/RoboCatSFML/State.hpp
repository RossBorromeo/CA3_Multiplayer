#pragma once

class State
{
public:
	virtual ~State() = default;
	virtual void HandleInput(const sf::Event& event) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(sf::RenderWindow& window) = 0;
};
