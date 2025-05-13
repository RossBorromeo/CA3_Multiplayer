#include "RoboCatPCH.hpp"

Yarn::Yarn()
	: mMuzzleSpeed(400.f),
	mVelocity(Vector3::Zero),
	mPlayerId(0)
{
	SetScale(GetScale() * 0.02f);
	SetCollisionRadius(15.f);
}

uint32_t Yarn::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	// Pose
	if (inDirtyState & EYRS_Pose)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(GetLocation().mX);
		inOutputStream.Write(GetLocation().mY);
		inOutputStream.Write(GetVelocity().mX);
		inOutputStream.Write(GetVelocity().mY);
		inOutputStream.Write(GetRotation());
		writtenState |= EYRS_Pose;
	}
	else
	{
		inOutputStream.Write(false);
	}

	// Color
	if (inDirtyState & EYRS_Color)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(GetColor());
		writtenState |= EYRS_Color;
	}
	else
	{
		inOutputStream.Write(false);
	}

	// Player ID
	if (inDirtyState & EYRS_PlayerId)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mPlayerId, 8);
		writtenState |= EYRS_PlayerId;
	}
	else
	{
		inOutputStream.Write(false);
	}

	return writtenState;
}

bool Yarn::HandleCollisionWithCat(RoboCat* /*inCat*/)
{
	// No special reaction — collisions handled elsewhere.
	return false;
}

void Yarn::InitFromShooter(RoboCat* inShooter)
{
	SetColor(inShooter->GetColor());
	SetPlayerId(inShooter->GetPlayerId());

	// Face "up" and rotate to match shooter
	sf::Vector2f shootDir(0.f, -1.f);
	thor::rotate(shootDir, inShooter->GetRotation());

	// Apply rotation direction to velocity
	SetVelocity(Vector3(shootDir.x, shootDir.y, 0.f) * mMuzzleSpeed);
	SetLocation(inShooter->GetLocation());
	SetRotation(inShooter->GetRotation());
}

void Yarn::Update()
{
	float deltaTime = Timing::sInstance.GetDeltaTime();
	SetLocation(GetLocation() + mVelocity * deltaTime);
}
