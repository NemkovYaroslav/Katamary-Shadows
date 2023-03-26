#include "GameObject.h"

#include "Transform.h"
#include "RenderComponent.h"
#include "RenderShadowsComponent.h"
#include "Component.h"

#include "Game.h"
#include "Camera.h"
#include "CameraController.h"

GameObject::GameObject(GameObject* parent)
{
	this->transformComponent = new TransformComponent();
	AddComponent(transformComponent);
	this->renderComponent = nullptr;
	this->collisionComponent = nullptr;
	this->radius = 0.0f;
	this->isUseLight = false;
}

GameObject::~GameObject()
{
	for (auto component : components)
	{
		delete component;
	}
	components.clear();
}

void GameObject::CreatePlane(float radius, std::string textureFileName)
{
	this->radius = radius;
	if (isUseLight)
	{
		renderComponent = new RenderComponent("../Shaders/LightShader.hlsl", textureFileName, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		renderComponent = new RenderComponent("../Shaders/TextureShader.hlsl", textureFileName, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	renderComponent->isLight = isUseLight;
	renderComponent->AddPlane(radius);
	AddComponent(renderComponent);
}

void GameObject::CreateMesh(float scaleRate, std::string textureFileName, std::string objectFileName)
{
	if (isUseLight)
	{
		renderComponent = new RenderComponent("../Shaders/LightShader.hlsl", textureFileName, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		renderComponent = new RenderComponent("../Shaders/TextureShader.hlsl", textureFileName, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	renderComponent->isLight = isUseLight;
	AddComponent(renderComponent);

	//renderShadowsComponent = new RenderShadowsComponent(renderComponent); //
	//AddComponent(renderShadowsComponent); //

	renderComponent->AddMesh(scaleRate, objectFileName);
}

void GameObject::Initialize()
{
	for (const auto& component : components)
	{
		component->Initialize();
	}
}

void GameObject::Update(float deltaTime)
{
	for (const auto& component : components)
	{
		component->Update(deltaTime);
	}
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
	component->gameObject = this;
}