class MouseServer : public Mouse
{
public:
    static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new MouseServer()); }

    void HandleDying() override;
    virtual bool HandleCollisionWithCat(RoboCat* inCat) override;
    void TakeDamage(int inAmount);
    virtual void Update() override;

protected:
    MouseServer();

private:
    int mHealth = 2;
    Vector3 mVelocity; //for mouse movement
    bool mDeathReported = false;

};
