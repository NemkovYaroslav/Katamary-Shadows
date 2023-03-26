#include "LightComponent.h"
#include "GameObject.h"
#include "Game.h"
#include "DisplayWin32.h"

void LightComponent::Initialize()
{

}

XMMATRIX LightComponent::GetViewMatrix()
{
	return gameObject->transformComponent->GetView();
}

XMMATRIX LightComponent::GetProjectionMatrix()
{
	return XMMatrixOrthographicLH(50.0f, 50.0f, 0.1f, 1000.0f);
	/*
	return Matrix::CreatePerspectiveFieldOfView(
		DirectX::XM_PIDIV2,
		Game::GetInstance()->GetDisplay()->GetClientWidth() / Game::GetInstance()->GetDisplay()->GetClientHeight(),
		0.1f,
		1000.0f
	);
	*/
}