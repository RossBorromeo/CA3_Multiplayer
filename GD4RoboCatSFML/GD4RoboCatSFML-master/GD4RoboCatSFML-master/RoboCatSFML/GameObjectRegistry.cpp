#include "RoboCatPCH.hpp"


std::unique_ptr< GameObjectRegistry >	GameObjectRegistry::sInstance;

void GameObjectRegistry::StaticInit()
{
	sInstance.reset(new GameObjectRegistry());
}

GameObjectRegistry::GameObjectRegistry()
{
}

void GameObjectRegistry::RegisterCreationFunction(uint32_t inFourCCName, GameObjectCreationFunc inCreationFunction)
{
	mNameToGameObjectCreationFunctionMap[inFourCCName] = inCreationFunction;
}

GameObjectPtr GameObjectRegistry::CreateGameObject(uint32_t inFourCCName)
{
	auto it = mNameToGameObjectCreationFunctionMap.find(inFourCCName);
	if (it == mNameToGameObjectCreationFunctionMap.end())
	{
		LOG("ERROR: Unknown GameObject type: %c%c%c%c (%u)",
			(inFourCCName >> 24) & 0xFF,
			(inFourCCName >> 16) & 0xFF,
			(inFourCCName >> 8) & 0xFF,
			(inFourCCName) & 0xFF,
			inFourCCName);

		return nullptr; // prevent crash
	}

	GameObjectCreationFunc creationFunc = it->second;
	GameObjectPtr gameObject = creationFunc();

	if (gameObject)
	{
		World::sInstance->AddGameObject(gameObject);
	}

	return gameObject;
}
