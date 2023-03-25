#pragma once

#include "LightComponent.h"

class RenderShadows
{
public:

	ID3D11Texture2D* lightDepthBufferTexture;
	ID3D11RenderTargetView* lightDepthBufferRenderTargetView;
	ID3D11ShaderResourceView* textureResourceView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

	ID3D11RasterizerState* rastState;
	ID3D11DepthStencilState* depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	LightComponent* light;

	RenderShadows();
	void Initialize();
	void Draw();
};