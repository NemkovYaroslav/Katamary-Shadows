#include "LightComponent.h"
#include "GameObject.h"

void LightComponent::Initialize()
{

}

XMMATRIX LightComponent::GetViewMatrix()
{
	return gameObject->transformComponent->GetView();
}

XMMATRIX LightComponent::GetProjectionMatrix()
{
	return XMMatrixOrthographicLH(1024.0f, 1024.0f, 0.1f, 1000.0f);
}