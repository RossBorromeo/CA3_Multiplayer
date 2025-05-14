#include "RoboCatClientPCH.hpp"

ConnectState::ConnectState(StateStack& stack) : State(stack) , mStack(stack)
{
	mFont.loadFromFile("../Assets/fonts/Carlito-Regular.ttf"); // Adjust if needed

	mIpLabel.setFont(mFont);
	mIpLabel.setString("Server IP:");
	mIpLabel.setPosition(600, 300);
	mIpLabel.setCharacterSize(24);

	mNameLabel.setFont(mFont);
	mNameLabel.setString("Your Name:");
	mNameLabel.setPosition(600, 360);
	mNameLabel.setCharacterSize(24);

	mConnectButton.setFont(mFont);
	mConnectButton.setString("Connect");
	mConnectButton.setPosition(800, 440);
	mConnectButton.setCharacterSize(32);

	mIpText.setFont(mFont);
	mIpText.setPosition(750, 300);
	mIpText.setCharacterSize(24);

	mNameText.setFont(mFont);
	mNameText.setPosition(750, 360);
	mNameText.setCharacterSize(24);

	mIpBox.setSize(sf::Vector2f(300, 30));
	mIpBox.setPosition(740, 295); // same as mIpText
	mIpBox.setFillColor(sf::Color::Transparent);
	mIpBox.setOutlineThickness(2);
	mIpBox.setOutlineColor(sf::Color::White);

	mNameBox = mIpBox;
	mNameBox.setPosition(740, 355);

	mFocus = Focus::IP;

}

void ConnectState::HandleInput(const sf::Event& event)
{
	std::cout << "[ConnectState] Event type: " << event.type << std::endl;

	if (event.type == sf::Event::KeyPressed)
	{
		std::cout << "[ConnectState] KeyPressed event detected: " << event.key.code << std::endl;

		if (event.key.code == sf::Keyboard::Down && mFocus == Focus::IP)
			mFocus = Focus::Name;
		else if (event.key.code == sf::Keyboard::Up && mFocus == Focus::Name)
			mFocus = Focus::IP;
		else if (event.key.code == sf::Keyboard::Enter)
		{
			std::cout << "[ConnectState] ENTER key pressed!" << std::endl;
			TryConnect();
		}
	}
	else if (event.type == sf::Event::TextEntered)
	{
		char c = static_cast<char>(event.text.unicode);
		if (std::isprint(c))
		{
			if (mFocus == Focus::IP) mIpInput += c;
			else if (mFocus == Focus::Name) mNameInput += c;
		}
		else if (event.text.unicode == 8) // backspace
		{
			if (mFocus == Focus::IP && !mIpInput.empty()) mIpInput.pop_back();
			else if (mFocus == Focus::Name && !mNameInput.empty()) mNameInput.pop_back();
		}
	}
}

void ConnectState::TryConnect()
{
	std::cerr << "[ConnectState] TryConnect called.\n";

	if (mIpInput.empty() || mNameInput.empty())
	{
		std::cerr << "[ConnectState] IP or Name is empty." << std::endl;
		return;
	}

	std::string ip = mIpInput;
	std::cerr << "[ConnectState] Original IP input: " << ip << std::endl;

	if (ip.find(':') == std::string::npos)
	{
		ip += ":50001";
	}
	std::cerr << "[ConnectState] Full IP string: " << ip << std::endl;

	SocketAddressPtr serverAddr = SocketAddressFactory::CreateIPv4FromString(ip);
	std::cerr << "[ConnectState] Address resolution finished." << std::endl;

	if (!serverAddr)
	{
		std::cerr << "[ConnectState] Failed to resolve IP: " << ip << std::endl;
		return;
	}

	std::cout << "[DEBUG] Host: " << serverAddr->ToString() << std::endl;

	std::cout << "[ConnectState] Connecting to " << ip << " as " << mNameInput << std::endl;
	NetworkManagerClient::StaticInit(*serverAddr, mNameInput);

	std::cerr << "[ConnectState] NetworkManagerClient::StaticInit done." << std::endl;

	mStack.RequestClear();
	mStack.RequestPush(std::make_unique<GameState>(mStack));

}



void ConnectState::Update(float)
{
	mIpText.setString(mIpInput);
	mNameText.setString(mNameInput);
}

void ConnectState::Render(sf::RenderWindow& window)
{
	window.clear(sf::Color::Black);

	if (mFocus == Focus::IP)
		mIpBox.setOutlineColor(sf::Color::Yellow);
	else
		mIpBox.setOutlineColor(sf::Color::White);

	if (mFocus == Focus::Name)
		mNameBox.setOutlineColor(sf::Color::Yellow);
	else
		mNameBox.setOutlineColor(sf::Color::White);

	window.draw(mIpBox);
	window.draw(mNameBox);

	window.draw(mIpLabel);
	window.draw(mIpText);
	window.draw(mNameLabel);
	window.draw(mNameText);
	window.draw(mConnectButton);
}
