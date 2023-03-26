#pragma once

#include "LightComponent.h"

class RenderShadowsComponent;

class RenderShadows
{
public:

	RenderShadows();
	void Draw();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> lightDepthBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> lightDepthBufferRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	/*
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	*/

	std::vector<RenderShadowsComponent*> renderShadowsComponents;
};