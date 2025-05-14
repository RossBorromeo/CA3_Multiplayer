#include "RoboCatClientPCH.hpp"

bool Client::StaticInit()
{
	Client* client = new Client();

	InputManager::StaticInit();
	WindowManager::StaticInit();
	FontManager::StaticInit();
	TextureManager::StaticInit();
	RenderManager::StaticInit();
	HUD::StaticInit();

	s_instance.reset(client);

	//  Push the menu state to start the UI
	client->GetStateStack().PushState(std::make_unique<MenuState>(client->GetStateStack()));

	return true;
}


Client::Client()
{
	GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatClient::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('MOUS', MouseClient::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('YARN', YarnClient::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('HPCK', HealthPickupClient::StaticCreate);

	//Might need this so DONT DELETE
	//string destination = StringUtils::GetCommandLineArg(1);
	//string name = StringUtils::GetCommandLineArg(2);

	//SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(destination);

	//NetworkManagerClient::StaticInit(*serverAddress, name);

	//NetworkManagerClient::sInstance->SetSimulatedLatency(0.0f);
}



void Client::DoFrame()
{
	sf::Event event;
	while (PollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			WindowManager::sInstance->close();
		}
			 
		mStateStack.HandleInput(event);
	}

	mStateStack.Update(Timing::sInstance.GetDeltaTime());
	mStateStack.Render(*WindowManager::sInstance);

	// Optional: Only run game logic if connected
	if (NetworkManagerClient::sInstance)
	{
		InputManager::sInstance->Update();
		NetworkManagerClient::sInstance->ProcessIncomingPackets();
		NetworkManagerClient::sInstance->SendOutgoingPackets();
		Engine::DoFrame();
		
		RenderManager::sInstance->Render();
		
	}

	//  CRUCIAL: Display the window!
	WindowManager::sInstance->display(); // <- Without this, the window stays white
}




void Client::HandleEvent(sf::Event& p_event)
{
	switch (p_event.type)
	{
	case sf::Event::KeyPressed:
		InputManager::sInstance->HandleInput(EIA_Pressed, p_event.key.code);
		break;
	case sf::Event::KeyReleased:
		InputManager::sInstance->HandleInput(EIA_Released, p_event.key.code);
		break;
	default:
		break;
	}
}

bool Client::PollEvent(sf::Event& p_event)
{
	return WindowManager::sInstance->pollEvent(p_event);
}


