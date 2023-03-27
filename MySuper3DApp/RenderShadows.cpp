#include "RenderShadows.h"

#include "DisplayWin32.h"
#include "RenderComponent.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderShadowsComponent.h"

RenderShadows::RenderShadows()
{
	viewport = std::make_shared<D3D11_VIEWPORT>();

	D3D11_TEXTURE2D_DESC textureDesc = {}; // ok
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = static_cast<float>(1024);
	textureDesc.Height = static_cast<float>(1024);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	auto result = Game::GetInstance()->GetRenderSystem()->device->CreateTexture2D(&textureDesc, nullptr, lightDepthBufferTexture.GetAddressOf());
	if (FAILED(result)) { return; }

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc; // ok
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateRenderTargetView(lightDepthBufferTexture.Get(), &renderTargetViewDesc, lightDepthBufferRenderTargetView.GetAddressOf());
	if (FAILED(result)) { return; }

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; // ok
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateShaderResourceView(lightDepthBufferTexture.Get(), &shaderResourceViewDesc, textureResourceView.GetAddressOf());

	/*
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc; // ??
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateDepthStencilState(&depthStencilStateDesc, depthStencilState.GetAddressOf());
	if (FAILED(result)) { return; }
	*/

	//
	/*
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

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = Game::GetInstance()->GetRenderSystem()->device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

	*/
	//

	D3D11_TEXTURE2D_DESC depthBufferDesc; // ok
	depthBufferDesc.Width = static_cast<float>(1024);
	depthBufferDesc.Height = static_cast<float>(1024);
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(result)) { return; }


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc; // ok
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
	if (FAILED(result)) { return; }


	viewport->Width = static_cast<float>(1024); // ok
	viewport->Height = static_cast<float>(1024);
	viewport->MinDepth = 0.0f;
	viewport->MaxDepth = 1.0f;
	viewport->TopLeftX = 0.0f;
	viewport->TopLeftY = 0.0f;
}

void RenderShadows::PrepareFrame()
{
	//Game::GetInstance()->GetRenderSystem()->context->ClearState();
	Game::GetInstance()->GetRenderSystem()->context->RSSetViewports(1, viewport.get());
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Game::GetInstance()->GetRenderSystem()->context->ClearRenderTargetView(lightDepthBufferRenderTargetView.Get(), bgcolor);
	Game::GetInstance()->GetRenderSystem()->context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderShadows::Draw()
{
	for (auto& renderShadowsComponent : renderShadowsComponents)
	{
		renderShadowsComponent->Draw();
	}
}

void RenderShadows::EndFrame()
{
	Game::GetInstance()->GetRenderSystem()->context->OMSetRenderTargets(0, nullptr, nullptr);
}