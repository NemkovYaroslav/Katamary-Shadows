#pragma once
#include "Component.h"
#include "RenderComponent.h"

using namespace DirectX::SimpleMath;

class RenderShadowsComponent : public Component
{
public:

    RenderComponent* currentRenderComponent;

    Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    //Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    //Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
 

    RenderShadowsComponent(RenderComponent* currentRenderComponent);
    RenderShadowsComponent() = delete;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;

    void Draw();
};

