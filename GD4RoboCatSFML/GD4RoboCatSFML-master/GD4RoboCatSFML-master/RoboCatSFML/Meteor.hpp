class Meteor : public GameObject
{
public:
	CLASS_IDENTIFICATION('METE', GameObject)

		enum EMeteorReplicationState
	{
		EMRS_Pose = 1 << 0,
		EMRS_Color = 1 << 1,
		EMRS_Health = 1 << 2,

		EMRS_AllState = EMRS_Pose | EMRS_Color | EMRS_Health
	};

	static GameObject* StaticCreate() { return new Meteor(); }

	virtual uint32_t GetAllStateMask() const override { return EMRS_AllState; }

	virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;
	virtual void Read(InputMemoryBitStream& inInputStream) override;

	virtual bool HandleCollisionWithShip(class Ship* inShip); // Will explode violently upon impact

	void SetHealth(int inHealth) { mHealth = inHealth; }
	int GetHealth() const { return mHealth; }

	void TakeDamage(int inDamage);

protected:
	Meteor();

private:
	int mHealth;
};