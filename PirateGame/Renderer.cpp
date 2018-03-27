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

sf::Vector2u CRenderer::GetWindowSize() const
{
	sf::Vector2u returnVec = myWindow->getSize();

	return std::move(returnVec);
}

sf::Vector2f CRenderer::GetViewSize() const
{
	sf::Vector2f returnVec = myWindow->getView().getSize();

	return std::move(returnVec);
}

const sf::RenderWindow & CRenderer::GetWindow() const
{
	return *myWindow;
}

void CRenderer::Render(sf::Drawable & aDrawable)
{
	myWindow->draw(aDrawable);
}

CRenderer::CRenderer()
	:myWindow(nullptr)
{
}
