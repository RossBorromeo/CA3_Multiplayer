#include "RoboCatClientPCH.hpp"

MenuState::MenuState(StateStack& stack) : State(stack), mStack(stack)
{
	mFont.loadFromFile("../Assets/fonts/Carlito-Regular.ttf");

	mOptions[0].setFont(mFont);
	mOptions[0].setString("Play");
	mOptions[0].setPosition(800, 400);
	mOptions[0].setCharacterSize(48);

	mOptions[1].setFont(mFont);
	mOptions[1].setString("Exit");
	mOptions[1].setPosition(800, 500);
	mOptions[1].setCharacterSize(48);

	// Highlight first by default
	mOptions[0].setFillColor(sf::Color::Yellow);
	mOptions[1].setFillColor(sf::Color::White);
}

void MenuState::UpdateSelection(int direction)
{
	mOptions[mSelectedIndex].setFillColor(sf::Color::White);
	mSelectedIndex += direction;

	if (mSelectedIndex < 0) mSelectedIndex = mOptions.size() - 1;
	if (mSelectedIndex >= mOptions.size()) mSelectedIndex = 0;

	mOptions[mSelectedIndex].setFillColor(sf::Color::Yellow);
}

void MenuState::ActivateSelection()
{
	if (mSelectedIndex == 0)
	{
		mStack.RequestPop(); // Safe!
		mStack.RequestPush(std::make_unique<ConnectState>(mStack));
	}
	else if (mSelectedIndex == 1)
	{
		exit(0); // Still okay
	}

}

void MenuState::HandleInput(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Up)
			UpdateSelection(-1);
		else if (event.key.code == sf::Keyboard::Down)
			UpdateSelection(1);
		else if (event.key.code == sf::Keyboard::Enter)
			ActivateSelection();
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

		for (int i = 0; i < mOptions.size(); ++i)
		{
			if (mOptions[i].getGlobalBounds().contains(mousePos))
			{
				mSelectedIndex = i;
				ActivateSelection();
			}
		}
	}
}

void MenuState::Update(float) {}

void MenuState::Render(sf::RenderWindow& window)
{
	window.clear(sf::Color::Black);
	for (auto& option : mOptions)
	{
		window.draw(option);
	}
}
