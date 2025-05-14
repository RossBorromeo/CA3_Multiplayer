#include "RoboCatServerPCH.hpp"

MouseServer::MouseServer()
{
    //Speed of Asteroids
    mVelocity = RoboMath::GetRandomVector(Vector3(-1.f, -1.f, 0.f), Vector3(1.f, 1.f, 0.f));
    mVelocity.Normalize();
    mVelocity *= 150.f; //Increased speed

}

void MouseServer::HandleDying()
{
    NetworkManagerServer::sInstance->UnregisterGameObject(this);

    GameObjectPtr pickup = GameObjectRegistry::sInstance->CreateGameObject('HPCK');
    pickup->SetLocation(GetLocation());
}



bool MouseServer::HandleCollisionWithCat(RoboCat* inCat)
{
    //Tkae 1hp damage
    static_cast<RoboCatServer*>(inCat)->TakeDamage(1);

    //kills the mouse too
    TakeDamage(5);
    
    return false; 
}

void MouseServer::TakeDamage(int inAmount)
{
    mHealth -= inAmount;

    NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), EMRS_Health);

    if (mHealth <= 0)
    {
        SetDoesWantToDie(true);
    }
}

void MouseServer::Update()
{
    float dt = Timing::sInstance.GetDeltaTime();
    Vector3 newPos = GetLocation() + mVelocity * dt;

    const float worldW = 1920.f;
    const float worldH = 1080.f;

    // Bounce off edges of the map
    if (newPos.mX < 0.f || newPos.mX > worldW)
    {
        mVelocity.mX *= -1.f;
        newPos.mX = std::clamp(newPos.mX, 0.f, worldW);
    }

    if (newPos.mY < 0.f || newPos.mY > worldH)
    {
        mVelocity.mY *= -1.f;
        newPos.mY = std::clamp(newPos.mY, 0.f, worldH);
    }

    SetLocation(newPos);
    NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), EMRS_Pose);
}


