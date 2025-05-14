#include "StateStack.hpp"

class Client : public Engine
{
	
public:

	static bool StaticInit();

	inline static std::unique_ptr<Client> s_instance;

	StateStack& GetStateStack() { return mStateStack; }

protected:

	Client();

	virtual void	DoFrame() override;
	virtual void	HandleEvent(sf::Event& p_event) override;
	virtual bool	PollEvent(sf::Event& p_event) override;

private:
	StateStack mStateStack;
};
