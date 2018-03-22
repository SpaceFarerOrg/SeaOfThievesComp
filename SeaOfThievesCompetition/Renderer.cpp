#include "Renderer.h"
#include <SFML\Graphics\RenderWindow.hpp>

CRenderer & CRenderer::GetInstance()
{
	static CRenderer instance;
	return instance;
}

void CRenderer::BindWindow(sf::RenderWindow & aWindow)
{
	myWindow = &aWindow;
}

sf::Vector2f CRenderer::GetViewCenter() const
{
	sf::Vector2f center = myWindow->getView().getCenter();

	return std::move(center);
}

void CRenderer::Render(sf::Drawable & aDrawable)
{
	myWindow->draw(aDrawable);
}

CRenderer::CRenderer()
	:myWindow(nullptr)
{
}
