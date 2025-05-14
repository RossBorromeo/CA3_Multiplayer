#include "RoboCatClientPCH.hpp"
#include "GameState.hpp"

GameState::GameState(StateStack& stack)
	: State(stack)
{
	InputManager::StaticInit();
}

void GameState::HandleInput(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		InputManager::sInstance->HandleInput(EIA_Pressed, event.key.code);
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		InputManager::sInstance->HandleInput(EIA_Released, event.key.code);
	}
}

void GameState::Update(float dt)
{
	if (!m_networkInitialised)
	{
		std::string destination = StringUtils::GetCommandLineArg(1);
		std::string name = StringUtils::GetCommandLineArg(2);

		SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(destination);
		NetworkManagerClient::StaticInit(*serverAddress, name);

		m_networkInitialised = true;
	}

	InputManager::sInstance->Update();
	World::sInstance->Update();
	NetworkManagerClient::sInstance->ProcessIncomingPackets();
	NetworkManagerClient::sInstance->SendOutgoingPackets();
}

void GameState::Render(sf::RenderWindow& window)
{
	std::cout << "[GameState] Rendering..." << std::endl;

	//  Clear the screen with a new color to show the state has changed
	window.clear(sf::Color::Blue);

	//  Optionally draw a message confirming this is GameState
	sf::Text debugText;
	debugText.setFont(*FontManager::sInstance->GetFont("carlito"));
	debugText.setString("GameState active!");
	debugText.setCharacterSize(32);
	debugText.setPosition(100, 100);
	debugText.setFillColor(sf::Color::White);
	window.draw(debugText);

	//  Then render the rest of your game scene
	RenderManager::sInstance->RenderComponents();
	HUD::sInstance->Render();
}

