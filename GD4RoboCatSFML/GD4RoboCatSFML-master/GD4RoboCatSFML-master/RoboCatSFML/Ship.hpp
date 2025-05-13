class Ship : public GameObject
{
public:
	CLASS_IDENTIFICATION('SHIP', GameObject)

		enum EShipReplicationState
	{
		ESRS_Pose = 1 << 0,
		ESRS_PlayerId = 1 << 1,
		ESRS_Health = 1 << 2,

		ESRS_AllState = ESRS_Pose | ESRS_PlayerId | ESRS_Health
	};

	static GameObject* StaticCreate() { return new Ship(); }

	virtual uint32_t GetAllStateMask() const override { return ESRS_AllState; }

	virtual void Update() override;

	void ProcessInput(float inDeltaTime, const InputState& inInputState);
	void SimulateMovement(float inDeltaTime);

	void ProcessCollisionsWithScreenWalls();

	void SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; }
	uint32_t GetPlayerId() const { return mPlayerId; }

	void SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
	const Vector3& GetVelocity() const { return mVelocity; }

	void TakeDamage(int inAmount);
	int GetHealth() const { return mHealth; }

	virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

protected:
	Ship();

private:
	void AdjustVelocityByThrust(float inDeltaTime);

	Vector3 mVelocity;
	uint32_t mPlayerId;
	float mMaxLinearSpeed;
	float mMaxRotationSpeed;
	float mThrustDir;
	float mWallRestitution;
	float mLastMoveTimestamp;
	int mHealth;
};

typedef shared_ptr< Ship >	ShipPtr;