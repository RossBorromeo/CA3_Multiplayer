#include "RoboCatPCH.hpp"


Meteor::Meteor()
	: mHealth(100) // Starting health for this spicy space rock
{
	SetScale(GetScale() * 1.5f);     // Meteor’s big and scary
	SetCollisionRadius(30.f);        // Big hitbox for dramatic impacts
	SetColor(Vector3(0.5f, 0.5f, 0.5f)); // Stylishly dull grey
}

bool Meteor::HandleCollisionWithShip(Ship* inShip)
{
	if (inShip)
	{
		inShip->TakeDamage(25); // The meteor's parting gift
		SetDoesWantToDie(true); // One-hit wonder
	}
	return false; // Continue checking other collisions if needed
}

void Meteor::TakeDamage(int inDamage)
{
	mHealth -= inDamage;
	if (mHealth <= 0)
	{
		SetDoesWantToDie(true);
	}
}

uint32_t Meteor::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	// Pose: Location + Rotation
	if (inDirtyState & EMRS_Pose)
	{
		inOutputStream.Write(true);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= EMRS_Pose;
	}
	else
	{
		inOutputStream.Write(false);
	}

	// Color
	if (inDirtyState & EMRS_Color)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(GetColor());

		writtenState |= EMRS_Color;
	}
	else
	{
		inOutputStream.Write(false);
	}

	// Health
	if (inDirtyState & EMRS_Health)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mHealth);

		writtenState |= EMRS_Health;
	}
	else
	{
		inOutputStream.Write(false);
	}

	return writtenState;
}

void Meteor::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;

	// Pose
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 location;
		inInputStream.Read(location.mX);
		inInputStream.Read(location.mY);
		SetLocation(location);

		float rotation;
		inInputStream.Read(rotation);
		SetRotation(rotation);
	}

	// Color
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 color;
		inInputStream.Read(color);
		SetColor(color);
	}

	// Health
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		int newHealth;
		inInputStream.Read(newHealth);
		SetHealth(newHealth);
	}
}