#pragma once

#include <memory>
#include <vector>             // Required for std::vector
#include <SFML/Graphics/RenderWindow.hpp>  // Required for sf::RenderWindow

#include "State.hpp"




class StateStack
{

public:
	enum class Action { Push, Pop, Clear };

	struct PendingChange
	{
		Action action;
		std::unique_ptr<State> state; // Only for Push

		PendingChange(Action a, std::unique_ptr<State> s = nullptr)
			: action(a), state(std::move(s)) {
		}
	};


	void PushState(std::unique_ptr<State> state);
	void PopState();
	void Clear();

	void RequestPush(std::unique_ptr<State> state);
	void RequestPop();
	void RequestClear();


	void HandleInput(const sf::Event& event);
	void Update(float dt);
	void Render(sf::RenderWindow& window);

	void ApplyPendingChanges();

private:
	std::vector<std::unique_ptr<State>> mStates;
	std::vector<PendingChange> mPendingChanges;

};

