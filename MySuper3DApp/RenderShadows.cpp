#include "RenderShadows.h"

#include "DisplayWin32.h"
#include "RenderComponent.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderShadowsComponent.h"

RenderShadows::RenderShadows()
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	//textureDesc.Width = Game::GetInstance()->GetDisplay()->GetClientWidth();
	//textureDesc.Height = Game::GetInstance()->GetDisplay()->GetClientHeight();
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
	if (FAILED(result))
		return;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = Game::GetInstance()->GetRenderSystem()->device->CreateRenderTargetView(lightDepthBufferTexture.Get(), &renderTargetViewDesc, lightDepthBufferRenderTargetView.GetAddressOf());
	if (FAILED(result))
		return;
	// пайплайн

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	result = Game::GetInstance()->GetRenderSystem()->device->CreateDepthStencilState(&depthStencilStateDesc, depthStencilState.GetAddressOf());
	if (FAILED(result)) { return; }

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

	result = Game::GetInstance()->GetRenderSystem()->device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32B32A32_FLOAT);
	result = Game::GetInstance()->GetRenderSystem()->device->CreateShaderResourceView(lightDepthBufferTexture.Get(), &srvDesc, textureResourceView.GetAddressOf());

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = static_cast<float>(1024);
	depthStencilDesc.Height = static_cast<float>(1024);
	//depthStencilDesc.Width = Game::GetInstance()->GetDisplay()->GetClientWidth();
	//depthStencilDesc.Height = Game::GetInstance()->GetDisplay()->GetClientHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	result = Game::GetInstance()->GetRenderSystem()->device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(result)) //If error occurred
	{
		return;
	}

	result = Game::GetInstance()->GetRenderSystem()->device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
	if (FAILED(result)) //If error occurred
	{
		return;
	}
}

void RenderShadows::Draw()
{
	//Game::GetInstance()->GetRenderSystem()->context->ClearState();
	Game::GetInstance()->GetRenderSystem()->context->OMSetRenderTargets(1, lightDepthBufferRenderTargetView.GetAddressOf(), depthStencilView.Get());
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Game::GetInstance()->GetRenderSystem()->context->ClearRenderTargetView(lightDepthBufferRenderTargetView.Get(), bgcolor);
	Game::GetInstance()->GetRenderSystem()->context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (auto& renderShadowsComponent : renderShadowsComponents)
	{
		renderShadowsComponent->Draw();
	}
	Game::GetInstance()->GetRenderSystem()->context->OMSetRenderTargets(0, nullptr, nullptr);
}