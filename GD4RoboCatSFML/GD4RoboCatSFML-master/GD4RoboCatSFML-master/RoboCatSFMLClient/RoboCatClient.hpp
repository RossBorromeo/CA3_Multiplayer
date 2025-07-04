class RoboCatClient : public RoboCat
{
public:
	static GameObjectPtr StaticCreate() { return GameObjectPtr(new RoboCatClient()); }

	virtual void Update();
	virtual void HandleDying() override;
	virtual void Read(InputMemoryBitStream& inInputStream) override;

	void DoClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState);
	void DoClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState);

	//Add this line:
	int GetHealth() const { return mHealth; }

protected:
	RoboCatClient();

private:
	void InterpolateClientSidePrediction(float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat);
	float mTimeLocationBecameOutOfSync;
	float mTimeVelocityBecameOutOfSync;

	float mTimeOfNextShootSound;
	float mTimeBetweenShootSounds;

	SpriteComponentPtr mSpriteComponent;
};


