#pragma once
#include "Component.h"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX::SimpleMath;

class RenderComponent : public Component
{
public:

    bool isLight;

    RenderComponent(std::string shaderFileName, D3D_PRIMITIVE_TOPOLOGY topology);
    RenderComponent(std::string shaderFileName, std::string textureFileName, D3D_PRIMITIVE_TOPOLOGY topology);
    RenderComponent() = delete;
    ~RenderComponent();

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;

    void Draw();
    void AddGrid(int gridSize, float cellSize, Color color);
    void AddPlane(float radius);
    void AddMesh(float scaleRate, std::string objectFileName);

    void ProcessNode(aiNode* node, const aiScene* scene, float scaleRate);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, float scaleRate);

protected:

    std::string shaderFileName;
    D3D_PRIMITIVE_TOPOLOGY topology;
    std::vector<Vector4> points;
    std::vector<int> indices;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>     pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>     inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>          vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>          indexBuffer;
    ID3D11Buffer** constBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;

    std::string textureFileName;
    Microsoft::WRL::ComPtr<ID3D11Resource> texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};
