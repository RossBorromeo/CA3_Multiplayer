#pragma once

#include "State.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class StateStack;

class ConnectState : public State
{
public:
	
	explicit ConnectState(StateStack& stack);
	void HandleInput(const sf::Event& event) override;
	void Update(float dt) override;
	void Render(sf::RenderWindow& window) override;

private:
	void TryConnect();

private:
	StateStack& mStack;
	sf::Font mFont;

	sf::Text mIpLabel, mNameLabel, mConnectButton;
	sf::Text mIpText, mNameText;

	std::string mIpInput;
	std::string mNameInput;

	sf::RectangleShape mIpBox;
	sf::RectangleShape mNameBox;


	enum class Focus { IP, Name,  };
	Focus mFocus;
};
