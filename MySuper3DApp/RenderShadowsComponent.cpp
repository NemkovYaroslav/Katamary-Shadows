#include "RenderShadowsComponent.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderShadows.h"
#include "LightComponent.h"
#include "GameObject.h"
#include "CameraComponent.h"

struct alignas(16) CameraLightData
{
	Matrix viewProjection;
	Matrix model;
};

RenderShadowsComponent::RenderShadowsComponent(RenderComponent* currentRenderComponent)
{
	this->currentRenderComponent = currentRenderComponent;
}

void RenderShadowsComponent::Initialize()
{
	Game::GetInstance()->GetRenderShadowsSystem()->renderShadowsComponents.push_back(this);

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	Game::GetInstance()->GetRenderSystem()->context->OMSetDepthStencilState(Game::GetInstance()->GetRenderShadowsSystem()->depthStencilState.Get(), 0);

	D3D11_BUFFER_DESC constBufferDesc = {};
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;
	constBufferDesc.ByteWidth = sizeof(CameraLightData);
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&constBufferDesc, nullptr, constBuffer.GetAddressOf());
	std::cout << "000" << std::endl;
}

void RenderShadowsComponent::Update(float deltaTime)
{
	const CameraLightData cameraLightData
	{
		Game::GetInstance()->currentLight->GetViewMatrix() * Game::GetInstance()->currentLight->GetProjectionMatrix(),
		gameObject->transformComponent->GetModel()
	};
	D3D11_MAPPED_SUBRESOURCE firstMappedResource;
	Game::GetInstance()->GetRenderSystem()->context->Map(constBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &firstMappedResource);
	memcpy(firstMappedResource.pData, &cameraLightData, sizeof(CameraLightData));
	Game::GetInstance()->GetRenderSystem()->context->Unmap(constBuffer.Get(), 0);
	std::cout << "111" << std::endl;
}

void RenderShadowsComponent::Draw()
{
	std::cout << "222" << std::endl;

	Game::GetInstance()->GetRenderSystem()->context->RSSetState(Game::GetInstance()->GetRenderShadowsSystem()->rastState.Get());
	
	Game::GetInstance()->GetRenderSystem()->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides[] = { 48 };
	UINT offsets[] = { 0 };

	Game::GetInstance()->GetRenderSystem()->context->IASetVertexBuffers(0, 1, currentRenderComponent->vertexBuffer.GetAddressOf(), strides, offsets); //

	Game::GetInstance()->GetRenderSystem()->context->VSSetShader(Game::GetInstance()->GetRenderShadowsSystem()->vertexShader.Get(), nullptr, 0);
	Game::GetInstance()->GetRenderSystem()->context->PSSetShader(Game::GetInstance()->GetRenderShadowsSystem()->pixelShader.Get(), nullptr, 0);

	Game::GetInstance()->GetRenderSystem()->context->IASetInputLayout(currentRenderComponent->inputLayout.Get());

	Game::GetInstance()->GetRenderSystem()->context->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());

	Game::GetInstance()->GetRenderSystem()->context->DrawIndexed(currentRenderComponent->indices.size(), 0, 0); //
}