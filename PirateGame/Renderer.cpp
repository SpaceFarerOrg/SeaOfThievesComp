#include "Renderer.h"
#include <SFML\Graphics\RenderWindow.hpp>
#include "Math.h"

CRenderer & CRenderer::GetInstance()
{
	static CRenderer instance;
	return instance;
}

void CRenderer::BindWindow(sf::RenderWindow & aWindow)
{
	myWindow = &aWindow;

	sf::Vector2f winSizeF = { (float)myWindow->getSize().x, (float)myWindow->getSize().y };
	myScreenSpaceView.setSize(winSizeF);
	myScreenSpaceView.setCenter(winSizeF / 2.f);

	myWorldSpaceView.setSize(winSizeF);
}

sf::Vector2f CRenderer::GetWorldViewCenter() const
{
	sf::Vector2f center = myWorldSpaceView.getCenter();

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

sf::Vector2f CRenderer::GetWindowCenter() const
{
	sf::Vector2f center = myScreenSpaceView.getCenter();

	return std::move(center);
}

const sf::RenderWindow & CRenderer::GetWindow() const
{
	return *myWindow;
}

void CRenderer::SetPosition(const sf::Vector2f & aPosition)
{
	myWorldSpaceView.setCenter(aPosition);
}

void CRenderer::SetPosition(float aX, float aY)
{
	SetPosition({ aX, aY });
}

void CRenderer::SetRotation(float aRotation)
{
	myTargetRotation = aRotation;
}

void CRenderer::RenderInScreenSpace(sf::Drawable & aDrawable)
{
	myWindow->setView(myScreenSpaceView);
	myWindow->draw(aDrawable);
}

void CRenderer::Update()
{
	float dt = myDeltaTimer.getElapsedTime().asSeconds();
	myDeltaTimer.restart();

	myCurrentRotation = Math::Lerp(myCurrentRotation, myTargetRotation, dt*2.f);
	myWorldSpaceView.setRotation(myCurrentRotation);
}

void CRenderer::Render(sf::Drawable & aDrawable)
{
	myWindow->setView(myWorldSpaceView);
	myWindow->draw(aDrawable);
}

CRenderer::CRenderer()
	:myWindow(nullptr)
	, myTargetRotation(0.f)
	,myCurrentRotation(0.f)
{
}
