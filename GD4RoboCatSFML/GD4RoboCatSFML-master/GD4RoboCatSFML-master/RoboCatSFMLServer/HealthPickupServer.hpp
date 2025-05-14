class HealthPickupServer : public GameObject
{
public:
    static GameObjectPtr StaticCreate()
    {
        return NetworkManagerServer::sInstance->RegisterAndReturn(new HealthPickupServer());
    }

    virtual void Update() override;
    virtual bool HandleCollisionWithCat(RoboCat* inCat) override;

protected:
    HealthPickupServer();

private:
    //auto-despawn after time
    float mTimeToDie = 10.f; 
};
