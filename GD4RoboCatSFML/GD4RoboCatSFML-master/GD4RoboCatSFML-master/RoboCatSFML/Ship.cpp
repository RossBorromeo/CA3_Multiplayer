#include "RoboCatPCH.hpp"


const float WORLD_HEIGHT = 720.f;
const float WORLD_WIDTH = 1280.f;

Ship::Ship()
	: mVelocity(Vector3::Zero),
	mPlayerId(0),
	mMaxLinearSpeed(400.f),
	mMaxRotationSpeed(100.f),
	mThrustDir(0.f),
	mWallRestitution(0.1f),
	mLastMoveTimestamp(0.f),
	mHealth(5)
{
	SetCollisionRadius(40.f); // Smaller than cats because ships are classy
	SetScale(1.0f);
}

void Ship::Update()
{
	// nothing automatic per-frame unless overridden later
}

void Ship::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
	float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * mMaxRotationSpeed * inDeltaTime;
	SetRotation(newRotation);

	mThrustDir = inInputState.GetDesiredVerticalDelta();
}

void Ship::AdjustVelocityByThrust(float inDeltaTime)
{
	Vector3 forwardVector = GetForwardVector();
	mVelocity = forwardVector * (mThrustDir * inDeltaTime * mMaxLinearSpeed);
}

void Ship::SimulateMovement(float inDeltaTime)
{
	AdjustVelocityByThrust(inDeltaTime);
	SetLocation(GetLocation() + mVelocity * inDeltaTime);
	ProcessCollisionsWithScreenWalls();
}

void Ship::ProcessCollisionsWithScreenWalls()
{
	Vector3 location = GetLocation();
	float x = location.mX;
	float y = location.mY;

	float vx = mVelocity.mX;
	float vy = mVelocity.mY;

	float radius = GetCollisionRadius();

	if ((y + radius) >= WORLD_HEIGHT && vy > 0)
	{
		mVelocity.mY = -vy * mWallRestitution;
		location.mY = WORLD_HEIGHT - radius;
	}
	else if (y - radius <= 0 && vy < 0)
	{
		mVelocity.mY = -vy * mWallRestitution;
		location.mY = radius;
	}

	if ((x + radius) >= WORLD_WIDTH && vx > 0)
	{
		mVelocity.mX = -vx * mWallRestitution;
		location.mX = WORLD_WIDTH - radius;
	}
	else if (x - radius <= 0 && vx < 0)
	{
		mVelocity.mX = -vx * mWallRestitution;
		location.mX = radius;
	}

	SetLocation(location);
}

void Ship::TakeDamage(int inAmount)
{
	mHealth -= inAmount;
	if (mHealth <= 0)
	{
		SetDoesWantToDie(true);
	}
}

uint32_t Ship::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	if (inDirtyState & ESRS_PlayerId)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(GetPlayerId());
		writtenState |= ESRS_PlayerId;
	}
	else
	{
		inOutputStream.Write(false);
	}

	if (inDirtyState & ESRS_Pose)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mVelocity.mX);
		inOutputStream.Write(mVelocity.mY);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= ESRS_Pose;
	}
	else
	{
		inOutputStream.Write(false);
	}

	if (inDirtyState & ESRS_Health)
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mHealth, 4); // Still packing it in nicely
		writtenState |= ESRS_Health;
	}
	else
	{
		inOutputStream.Write(false);
	}

	return writtenState;
}