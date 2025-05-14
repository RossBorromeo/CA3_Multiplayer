#pragma once
#include "State.hpp"
#include <SFML/Graphics.hpp>
#include <array>

class MenuState : public State
{
public:
	
	explicit MenuState(StateStack& stack);
	void HandleInput(const sf::Event& event) override;
	void Update(float dt) override;
	void Render(sf::RenderWindow& window) override;

private:
	void UpdateSelection(int direction); // -1 for up, +1 for down
	void ActivateSelection();            // Enter or mouse click

private:
	StateStack& mStack;
	sf::Font mFont;

	std::array<sf::Text, 2> mOptions;
	int mSelectedIndex = 0;
};
