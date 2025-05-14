class HealthPickupClient : public GameObject
{
public:
    CLASS_IDENTIFICATION('HPCK', HealthPickupClient)

    static GameObjectPtr StaticCreate()
    {
        return GameObjectPtr(new HealthPickupClient());
    }

    virtual void Update() override;

protected:
    HealthPickupClient();

private:
    std::unique_ptr<SpriteComponent> mSpriteComponent;
};
