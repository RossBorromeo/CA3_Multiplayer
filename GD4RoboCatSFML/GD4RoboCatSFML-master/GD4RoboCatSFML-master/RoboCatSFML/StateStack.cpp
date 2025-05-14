#include "RoboCatPCH.hpp"


void StateStack::PushState(std::unique_ptr<State> state) { std::cout << "Pushing state of type: " << typeid(*state).name() << std::endl; mStates.push_back(std::move(state)); }
void StateStack::PopState() { if (!mStates.empty()) mStates.pop_back(); }
void StateStack::Clear() { mStates.clear(); }

void StateStack::RequestPush(std::unique_ptr<State> state)
{
	mPendingChanges.emplace_back(Action::Push, std::move(state));
}

void StateStack::RequestPop()
{
	mPendingChanges.emplace_back(Action::Pop);
}

void StateStack::RequestClear()
{
	mPendingChanges.emplace_back(Action::Clear);
}

void StateStack::HandleInput(const sf::Event& event)
{
	std::cout << "[StateStack] Forwarding event: " << event.type << std::endl;
	if (!mStates.empty())
	{
		mStates.back()->HandleInput(event);
	}
		
}

void StateStack::Update(float dt)
{
	if (!mStates.empty())
		mStates.back()->Update(dt);

	// Apply changes **after** update loop
	for (auto& change : mPendingChanges)
	{
		switch (change.action)
		{
		case Action::Push:
			mStates.push_back(std::move(change.state));
			break;
		case Action::Pop:
			if (!mStates.empty())
				mStates.pop_back();
			break;
		case Action::Clear:
			mStates.clear();
			break;
		}
	}
	mPendingChanges.clear();
}


void StateStack::Render(sf::RenderWindow& window)
{
	//  Ensure state changes are applied before rendering
	if (!mPendingChanges.empty())
	{
		for (auto& change : mPendingChanges)
		{
			switch (change.action)
			{
			case Action::Push:
				mStates.push_back(std::move(change.state));
				break;
			case Action::Pop:
				if (!mStates.empty())
					mStates.pop_back();
				break;
			case Action::Clear:
				mStates.clear();
				break;
			}
		}
		mPendingChanges.clear();
	}

	//  Only render the top state
	if (!mStates.empty())
		mStates.back()->Render(window);
}

void StateStack::ApplyPendingChanges()
{
	for ( PendingChange& change : mPendingChanges)
	{
		switch (change.action)
		{
		case Action::Push:
			if (change.state)
			{
				mStates.push_back(std::move(change.state));
			}
			break;
		case Action::Pop:
			if (!mStates.empty())
			{
				mStates.pop_back();
			}
			break;
		case Action::Clear:
			mStates.clear();
			break;
		}
	}
	mPendingChanges.clear();
}






