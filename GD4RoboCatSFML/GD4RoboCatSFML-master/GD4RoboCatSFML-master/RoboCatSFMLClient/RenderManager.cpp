#include "RoboCatClientPCH.hpp"

template<typename T>
const T& clamp(const T& v, const T& lo, const T& hi)
{
	return (v < lo) ? lo : (v > hi) ? hi : v;
}

constexpr float worldWidth = 1920.f;
constexpr float worldHeight = 1080.f;


std::unique_ptr<RenderManager> RenderManager::sInstance;

RenderManager::RenderManager()
{
	view.reset(sf::FloatRect(0, 0, 1920, 1080));
	view.zoom(0.5f);
	mLastPlayerPosition = sf::Vector2f(0.f, 0.f); // start at origin

	WindowManager::sInstance->setView(view);
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

void RenderManager::UpdateCamera()
{
	sf::Vector2f playerPos = FindCatCentre();

	if (playerPos != sf::Vector2f(-1.f, -1.f))
	{
		constexpr float followSpeed = 0.1f;
		const sf::Vector2f currentCenter = view.getCenter();
		const sf::Vector2f targetCenter = currentCenter + (playerPos - currentCenter) * followSpeed;

		// Get view size
		const sf::Vector2f viewSize = view.getSize();

		// Clamp the new camera center so it doesn't go out of bounds
		sf::Vector2f clampedCenter = targetCenter;

		const float halfWidth = viewSize.x / 2.f;
		const float halfHeight = viewSize.y / 2.f;

		clampedCenter.x = clamp(clampedCenter.x, halfWidth, worldWidth - halfWidth);
		clampedCenter.y = clamp(clampedCenter.y, halfHeight, worldHeight - halfHeight);

		view.setCenter(clampedCenter);
		WindowManager::sInstance->setView(view);
	}
}


// Way of finding this clients cat, and then centre point. - Ronan
sf::Vector2f RenderManager::FindCatCentre()
{
	uint32_t catID = (uint32_t)'RCAT';
	for (auto obj : World::sInstance->GetGameObjects())
	{
		// Find a cat.
		if (obj->GetClassId() == catID)
		{
			RoboCat* cat = dynamic_cast<RoboCat*>(obj.get());
			auto id = cat->GetPlayerId();
			auto ourID = NetworkManagerClient::sInstance->GetPlayerId();
			if (id == ourID)
			{
				// If so grab the centre point.
				auto centre = cat->GetLocation();
				m_lastCatPos.x = centre.mX;
				m_lastCatPos.y = centre.mY;
				return sf::Vector2f(centre.mX, centre.mY);
			}
		}
	}
}

void RenderManager::RenderComponents()
{
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

			//Health above cat 
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

				sf::FloatRect bounds = text.getLocalBounds();
				text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
				text.setPosition(textLoc.mX, textLoc.mY);

				WindowManager::sInstance->draw(text);
			}

			//Score below cat 
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

void RenderManager::Render()
{
	UpdateCamera();

	// Clear back buffer
	WindowManager::sInstance->clear(sf::Color(100, 149, 237, 255));

	// Draw background first
	sf::Sprite bgSprite;
	bgSprite.setTexture(*TextureManager::sInstance->GetTexture("Space"));
	bgSprite.setPosition(0.f, 0.f);
	WindowManager::sInstance->draw(bgSprite);

	// Then draw sprites and components
	RenderComponents();

	// Draw HUD on top
	HUD::sInstance->Render();



	// Present to screen
	WindowManager::sInstance->display();
}


