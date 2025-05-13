class MouseClient : public Mouse
{
public:
	static GameObjectPtr StaticCreate() { return GameObjectPtr(new MouseClient()); }

	//Getter to access health (needed for rendering)
	int GetHealth() const { return mHealth; }

protected:
	MouseClient();

protected:
	int mHealth = 5;


private:
	SpriteComponentPtr mSpriteComponent;
};