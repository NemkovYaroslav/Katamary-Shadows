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

	//

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	ID3DBlob* errorCode = nullptr;
	auto res = D3DCompileFromFile(
		L"../Shaders/DepthShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		vertexShaderByteCode.GetAddressOf(),
		&errorCode
	);

	if (FAILED(res))
	{
		if (errorCode)
		{
			const char* compileErrors = (char*)(errorCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			std::cout << "Missing Shader File: " << std::endl;
		}
		return;
	}

	Game::GetInstance()->GetRenderSystem()->device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr, vertexShader.GetAddressOf()
	);

	/*
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;
	res = D3DCompileFromFile(
		L"../Shaders/DepthShader.hlsl",
		nullptr,
		nullptr,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		pixelShaderByteCode.GetAddressOf(),
		&errorCode
	);
	Game::GetInstance()->GetRenderSystem()->device->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr, pixelShader.GetAddressOf()
	);
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
			D3D11_INPUT_ELEMENT_DESC {
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			D3D11_INPUT_ELEMENT_DESC {
				"TEXCOORD",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			D3D11_INPUT_ELEMENT_DESC {
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
	};
	Game::GetInstance()->GetRenderSystem()->device->CreateInputLayout(
		inputElements,
		3,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		inputLayout.GetAddressOf()
	);
	*/

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	res = Game::GetInstance()->GetRenderSystem()->device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	Game::GetInstance()->GetRenderSystem()->context->OMSetDepthStencilState(Game::GetInstance()->GetRenderShadowsSystem()->depthStencilState.Get(), 0);

	//

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

	Game::GetInstance()->GetRenderSystem()->context->PSSetShaderResources(0, 1, currentRenderComponent->textureView.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetSamplers(0, 1, currentRenderComponent->samplerState.GetAddressOf());

	Game::GetInstance()->GetRenderSystem()->context->RSSetState(rastState.Get());

	//Game::GetInstance()->GetRenderSystem()->context->IASetInputLayout(inputLayout.Get());
	
	Game::GetInstance()->GetRenderSystem()->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Game::GetInstance()->GetRenderSystem()->context->IASetIndexBuffer(currentRenderComponent->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	//Game::GetInstance()->GetRenderSystem()->context->IASetVertexBuffers(0, 1, currentRenderComponent->vertexBuffer.GetAddressOf(), strides, offsets);

	Game::GetInstance()->GetRenderSystem()->context->VSSetShader(vertexShader.Get(), nullptr, 0);
	
	//Game::GetInstance()->GetRenderSystem()->context->PSSetShader(Game::GetInstance()->GetRenderShadowsSystem()->pixelShader.Get(), nullptr, 0); // серый цвет

	Game::GetInstance()->GetRenderSystem()->context->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());

	Game::GetInstance()->GetRenderSystem()->context->DrawIndexed(currentRenderComponent->indices.size(), 0, 0);
}