#include "RoboCatClientPCH.hpp"

HealthPickupClient::HealthPickupClient()
{
    SetScale(0.5f);
    SetCollisionRadius(20.f);
    SetColor(Vector3(0.f, 1.f, 0.f)); 

    mSpriteComponent = std::make_unique<SpriteComponent>(this);
    mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("Health"));
}

void HealthPickupClient::Update()
{
}
