#pragma once
#include "includes.h"

class TransformComponent;
class RenderComponent;
class CollisionComponent;
class Component;

using namespace DirectX::SimpleMath;

class GameObject
{
public:

	TransformComponent* transformComponent;
	RenderComponent* renderComponent;
	CollisionComponent* collisionComponent;

	std::vector<Component*> components;

	bool isUseLight;
	float radius;
	float maxRadius = 0;

	GameObject(GameObject* parent = nullptr);
	~GameObject();

	virtual void Update(float deltaTime);
	virtual void Initialize();

	void AddComponent(Component* component);

	void CreatePlane(float radius, std::string textureFileName);
	void CreateMesh(float scaleRate, std::string textureFileName, std::string objectFileName);
};