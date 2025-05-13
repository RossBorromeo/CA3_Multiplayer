#include "RoboCatServerPCH.hpp"


MouseServer::MouseServer()
{
}


void MouseServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}


bool MouseServer::HandleCollisionWithCat(RoboCat* inCat)
{
	// Damage the player by 1 HP
	static_cast<RoboCatServer*>(inCat)->TakeDamage(1);

	// (Optional) Damage or kill the mouse too
	TakeDamage(5); // Uncomment if you want mouse to take damage too

	return false; // Don't destroy the mouse by default
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









