class MouseServer : public Mouse
{
public:
    static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new MouseServer()); }

    void HandleDying() override;
    virtual bool HandleCollisionWithCat(RoboCat* inCat) override;
    void TakeDamage(int inAmount);

protected:
    MouseServer();

private:
    int mHealth = 5;
};

