#include "RoboCatServerPCH.hpp"


YarnServer::YarnServer()
{
	//yarn lives a second...
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + 1.f;
}

void YarnServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}


void YarnServer::Update()
{
    Yarn::Update();

    if (Timing::sInstance.GetFrameStartTime() > mTimeToDie)
    {
        SetDoesWantToDie(true);
        return;
    }

    // Check collision with mice
    for (auto& gameObject : World::sInstance->GetGameObjects())
    {
        if (gameObject->GetClassId() == MouseServer::kClassId)
        {
            MouseServer* mouse = static_cast<MouseServer*>(gameObject.get());

            float distSq = (GetLocation() - mouse->GetLocation()).LengthSq();
            const float kMouseCollisionDist = 25.f;
            if (distSq < kMouseCollisionDist * kMouseCollisionDist)
            {
                mouse->TakeDamage(1); // each yarn does 1 damage
                SetDoesWantToDie(true); // yarn disappears
                break;
            }
        }
    }
}


bool YarnServer::HandleCollisionWithCat(RoboCat* inCat)
{
	if (inCat->GetPlayerId() != GetPlayerId())
	{
		//kill yourself!
		SetDoesWantToDie(true);

		static_cast<RoboCatServer*>(inCat)->TakeDamage(GetPlayerId());

	}

	return false;
}




