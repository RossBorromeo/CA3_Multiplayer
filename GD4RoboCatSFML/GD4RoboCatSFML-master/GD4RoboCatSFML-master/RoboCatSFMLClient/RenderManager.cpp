#include "RoboCatClientPCH.hpp"

std::unique_ptr< RenderManager >	RenderManager::sInstance;

RenderManager::RenderManager()
{
	view.reset(sf::FloatRect(0, 0, 1920, 1080));
	//WindowManager::sInstance->setView(view);

	if (const auto& player = NetworkManagerClient::sInstance->GetGameObject(NetworkManagerClient::sInstance->GetPlayerId()))
	{
		view.setCenter(player->GetLocation().mX, player->GetLocation().mY);
		WindowManager::sInstance->setView(view);
	}
}




void RenderManager::StaticInit()
{
	sInstance.reset(new RenderManager());
}


void RenderManager::AddComponent(SpriteComponent* inComponent)
{
	mComponents.emplace_back(inComponent);
}

void RenderManager::RemoveComponent(SpriteComponent* inComponent)
{
	int index = GetComponentIndex(inComponent);

	if (index != -1)
	{
		int lastIndex = mComponents.size() - 1;
		if (index != lastIndex)
		{
			mComponents[index] = mComponents[lastIndex];
		}
		mComponents.pop_back();
	}
}

int RenderManager::GetComponentIndex(SpriteComponent* inComponent) const
{
	for (int i = 0, c = mComponents.size(); i < c; ++i)
	{
		if (mComponents[i] == inComponent)
		{
			return i;
		}
	}

	return -1;
}


//this part that renders the world is really a camera-
//in a more detailed engine, we'd have a list of cameras, and then render manager would
//render the cameras in order
void RenderManager::RenderComponents()
{
	//Get the logical viewport so we can pass this to the SpriteComponents when it's draw time
	for (SpriteComponent* c : mComponents)
	{	
		WindowManager::sInstance->draw(c->GetSprite());	
	}

	for (const auto& gameObject : World::sInstance->GetGameObjects())
	{
		RoboCatClient* robocat = dynamic_cast<RoboCatClient*>(gameObject.get());

		MouseClient* mouse = dynamic_cast<MouseClient*>(gameObject.get());
		if (mouse)
		{
			Vector3 loc = mouse->GetLocation();
			Vector3 textLoc = loc + Vector3(0.f, -40.f, 0.f); 

			int health = mouse->GetHealth();
		}

		if (robocat)
		{
			Vector3 loc = robocat->GetLocation();
<<<<<<< Updated upstream
			Vector3 textLoc = loc + Vector3(0.f, -50.f, 0.f); // 50 units above the cat
=======
>>>>>>> Stashed changes

			// ---- Health above cat ----
			Vector3 textLoc = loc + Vector3(0.f, -50.f, 0.f);
			int health = robocat->GetHealth();

			if (health > 0)
			{
				std::string healthStr = StringUtils::Sprintf("HP: %d", health);

				sf::Text text;
				text.setString(healthStr);
				text.setFillColor(sf::Color::White);
				text.setCharacterSize(20);
				text.setFont(*FontManager::sInstance->GetFont("Pixel"));

				//Center the origin of the text
				sf::FloatRect textBounds = text.getLocalBounds();
				text.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);

				text.setPosition(textLoc.mX, textLoc.mY);
				WindowManager::sInstance->draw(text);
			}

			// ---- Score below cat ----
			int playerId = robocat->GetPlayerId();
			auto entry = ScoreBoardManager::sInstance->GetEntry(playerId);
			if (entry)
			{
				std::string scoreStr = StringUtils::Sprintf("Score: %d", entry->GetScore());
				Vector3 scoreLoc = loc + Vector3(0.f, 40.f, 0.f); // 40 units below

				sf::Text scoreText;
				scoreText.setString(scoreStr);
				scoreText.setFillColor(sf::Color::White);
				scoreText.setCharacterSize(18);
				scoreText.setFont(*FontManager::sInstance->GetFont("Pixel"));

				sf::FloatRect scoreBounds = scoreText.getLocalBounds();
				scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
				scoreText.setPosition(scoreLoc.mX, scoreLoc.mY);

				WindowManager::sInstance->draw(scoreText);
			}
		}
	}
}

<<<<<<< Updated upstream
void RenderManager::Render()
{
	// Clear back buffer
	WindowManager::sInstance->clear(sf::Color(100, 149, 237, 255));

	//Draw background first
	sf::Sprite bgSprite;
	bgSprite.setTexture(*TextureManager::sInstance->GetTexture("Space"));
	bgSprite.setPosition(0.f, 0.f);
	WindowManager::sInstance->draw(bgSprite);

	// Then draw sprites and components
	RenderComponents();

	//Then draw HUD (always on top)
	HUD::sInstance->Render();

	// Present back buffer to screen
	WindowManager::sInstance->display();
}


=======
>>>>>>> Stashed changes
