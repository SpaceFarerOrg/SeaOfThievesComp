#include "Compass.h"
#include "Renderer.h"
#include "TextureBank.h"

void CCompass::Init()
{
	myCompassBase.setTexture(CTextureBank::GetInstance().GetTexture(ETexture::CompassBase));
	myCompassPointer.setTexture(CTextureBank::GetInstance().GetTexture(ETexture::CompassPointer));

	myCompassBase.setOrigin(myCompassBase.getLocalBounds().width / 2.f, myCompassBase.getLocalBounds().height / 2.f);

	myCompassBase.setPosition(CRenderer::GetInstance().GetViewSize() - myCompassBase.getOrigin());
	myCompassPointer.setPosition(myCompassBase.getPosition());
}

void CCompass::Update(float aDT, float aPlayerRotation)
{
	myCompassPointer.setRotation(aPlayerRotation);
}

void CCompass::Render()
{
	CRenderer::GetInstance().RenderInScreenSpace(myCompassBase);
	CRenderer::GetInstance().RenderInScreenSpace(myCompassPointer);
}
