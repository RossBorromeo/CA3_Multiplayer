#pragma once



class SpriteComponent;
class RenderManager
{
public:
    static void StaticInit();
    static std::unique_ptr<RenderManager> sInstance;

    void Render();
    void RenderComponents();

    void AddComponent(SpriteComponent* inComponent);
    void RemoveComponent(SpriteComponent* inComponent);
    int GetComponentIndex(SpriteComponent* inComponent) const;

private:
    RenderManager();

    void UpdateCamera();
    sf::Vector2f FindCatCentre(); // <-- ADDED: Helper to get player position safely

    std::vector<SpriteComponent*> mComponents;
    sf::View view;

    sf::Vector2f mLastPlayerPosition; // For smooth interpolation fallback
    sf::Vector2f m_lastCatPos;

};
