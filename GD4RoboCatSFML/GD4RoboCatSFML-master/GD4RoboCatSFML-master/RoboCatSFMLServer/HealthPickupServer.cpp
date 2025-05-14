#include "RoboCatServerPCH.hpp"

HealthPickupServer::HealthPickupServer()
{
    SetScale(0.5f);
    SetCollisionRadius(20.f);
    //Colour Green
    SetColor(Vector3(0.f, 1.f, 0.f));
}

void HealthPickupServer::Update()
{
    mTimeToDie -= Timing::sInstance.GetDeltaTime();
    if (mTimeToDie <= 0.f)
    {
        SetDoesWantToDie(true);
    }
}

bool HealthPickupServer::HandleCollisionWithCat(RoboCat* inCat)
{
    if (RoboCatServer* catServer = static_cast<RoboCatServer*>(inCat))
    {
        // Heal Player
        catServer->TakeDamage(-1); 
    }

    //Despawn after pickup
    SetDoesWantToDie(true); 
    return false;
}
