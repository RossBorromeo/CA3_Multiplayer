#include "RoboCatServerPCH.hpp"
#include <iostream>
#include <random>

// Spawn timing variables
float gTimeSinceLastMouseSpawn = 0.f;
// spawn every 2 seconds
const float kMouseSpawnInterval = 2.f; 

bool Server::StaticInit()
{
	s_instance.reset(new Server());

	return true;
}

Server::Server()
{

	GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('MOUS', MouseServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('YARN', YarnServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('HPCK', HealthPickupServer::StaticCreate);


	InitNetworkManager();

	// Setup latency
	float latency = 0.0f;
	string latencyString = StringUtils::GetCommandLineArg(2);
	if (!latencyString.empty())
	{
		latency = stof(latencyString);
	}
	NetworkManagerServer::sInstance->SetSimulatedLatency(latency);
}


int Server::Run()
{
	SetupWorld();

	return Engine::Run();
}

bool Server::InitNetworkManager()
{
	string portString = StringUtils::GetCommandLineArg(1);
	uint16_t port = stoi(portString);

	return NetworkManagerServer::StaticInit(port);
}

GameObjectPtr CreateSingleMouse()
{
	Vector3 mouseMin(100.f, 100.f, 0.f);
	Vector3 mouseMax(1180.f, 620.f, 0.f);

	GameObjectPtr go = GameObjectRegistry::sInstance->CreateGameObject('MOUS');
	Vector3 mouseLocation = RoboMath::GetRandomVector(mouseMin, mouseMax);
	go->SetLocation(mouseLocation);

	return go;
}



namespace
{

	void CreateRandomMice(int inMouseCount)
	{
		for (int i = 0; i < inMouseCount; ++i)
		{
			CreateSingleMouse();
		}
	}
}


void Server::SetupWorld()
{
	//spawn some random mice
	CreateRandomMice(10);

	//spawn more random mice!
	//CreateRandomMice(10);
}

void Server::DoFrame()
{
	NetworkManagerServer::sInstance->ProcessIncomingPackets();

	NetworkManagerServer::sInstance->CheckForDisconnects();

	NetworkManagerServer::sInstance->RespawnCats();

	Engine::DoFrame();

	//Infinite Mouse Spawning
	gTimeSinceLastMouseSpawn += Timing::sInstance.GetDeltaTime();
	if (gTimeSinceLastMouseSpawn > kMouseSpawnInterval)
	{
		CreateSingleMouse();
		gTimeSinceLastMouseSpawn = 0.f;
	}


	NetworkManagerServer::sInstance->SendOutgoingPackets();

}

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{

	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->AddEntry(playerId, inClientProxy->GetName());
	SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
	RoboCatPtr cat = std::static_pointer_cast<RoboCat>(GameObjectRegistry::sInstance->CreateGameObject('RCAT'));
	cat->SetColor(ScoreBoardManager::sInstance->GetEntry(inPlayerId)->GetColor());
	cat->SetPlayerId(inPlayerId);
	//gotta pick a better spawn location than this...
	float margin = 100.f; 
	float worldW = 1920.f;
	float worldH = 1080.f;

	float x = margin + static_cast<float>(rand()) / RAND_MAX * (worldW - 2 * margin);
	float y = margin + static_cast<float>(rand()) / RAND_MAX * (worldH - 2 * margin);

	cat->SetLocation(Vector3(x, y, 0.f));
}


void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
	//kill client's cat
	//remove client from scoreboard
	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->RemoveEntry(playerId);
	RoboCatPtr cat = GetCatForPlayer(playerId);
	if (cat)
	{
		cat->SetDoesWantToDie(true);
	}
}

RoboCatPtr Server::GetCatForPlayer(int inPlayerId)
{
	//run through the objects till we find the cat...
	//it would be nice if we kept a pointer to the cat on the clientproxy
	//but then we'd have to clean it up when the cat died, etc.
	//this will work for now until it's a perf issue
	const auto& gameObjects = World::sInstance->GetGameObjects();
	for (int i = 0, c = gameObjects.size(); i < c; ++i)
	{
		GameObjectPtr go = gameObjects[i];
		RoboCat* cat = go->GetAsCat();
		if (cat && cat->GetPlayerId() == inPlayerId)
		{
			return std::static_pointer_cast<RoboCat>(go);
		}
	}

	return nullptr;

}
