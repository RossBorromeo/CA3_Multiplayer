#include "RoboCatPCH.hpp"

Mouse::Mouse()
{
	SetScale(GetScale() * 0.5f);
	SetCollisionRadius(30.f);
}

bool Mouse::HandleCollisionWithCat(RoboCat* inCat)
{
	(void)inCat;
	return false;
}

uint32_t Mouse::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	if (inDirtyState & EMRS_Pose)
	{
		inOutputStream.Write((bool)true);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= EMRS_Pose;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	if (inDirtyState & EMRS_Color)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetColor());

		writtenState |= EMRS_Color;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//Add health replication
	if (inDirtyState & EMRS_Health)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(mHealth);
		writtenState |= EMRS_Health;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}


	return writtenState;
}

void Mouse::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;

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

	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 color;
		inInputStream.Read(color);
		SetColor(color);
	}

	//Read health on client side
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		inInputStream.Read(mHealth);
	}
}
