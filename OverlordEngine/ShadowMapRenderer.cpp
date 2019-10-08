#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "TransformComponent.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	delete m_pShadowRT;
	delete m_pShadowMat;

}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{


	if (m_IsInitialized)
		return;

	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	RENDERTARGET_DESC desc{};
	desc.Width = 1280;
	desc.Height = 720;
	desc.EnableColorBuffer = false;
	desc.EnableColorSRV = false;
	desc.EnableDepthBuffer = true;
	desc.EnableDepthSRV = true;
	desc.IsValid();
	if (FAILED(m_pShadowRT->Create(desc))) Logger::LogError(L"Failed to create RenderTarget!\n");

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(direction);
	//TODO: store the input parameters in the appropriate datamembers
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
	
	m_LightPosition = position;
	m_LightDirection = direction;

	auto windowSettings = OverlordGame::GetGameSettings().Window;
	const float vwidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
	const float vheight = (m_Size > 0) ? m_Size : windowSettings.Height;

	auto world = DirectX::XMMatrixTranslation(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);

	auto up = DirectX::XMFLOAT3(0, 1, 0);
	auto forward = DirectX::XMLoadFloat3(&m_LightDirection);
	auto right = DirectX::XMVector3Cross(forward, DirectX::XMLoadFloat3(&up));
	auto newUp = DirectX::XMVector3Cross(right, forward);

	auto view = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_LightPosition), forward, newUp);
	auto proj = DirectX::XMMatrixOrthographicLH(vwidth, vheight, 0.1f, 400.0f); 
	DirectX::XMStoreFloat4x4(&m_LightVP, view * proj);


}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	float white[4]{ 0,0,0,0 };
	m_pShadowRT->Clear(gameContext, white);
	//TODO: set the shader variables of this shadow generator material
	if (m_pShadowMat)
	{
		m_pShadowMat->SetLightVP(m_LightVP);

		m_pShadowMat->SetWorld(gameContext.pCamera->GetTransform()->GetWorld());

		m_pShadowMat->SetBones(nullptr, 0);
	}
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{

	UNREFERENCED_PARAMETER(gameContext);
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	if (!m_pShadowMat)
	{
		Logger::LogWarning(L"ShadowMapRenderer::Draw() > No ShadowMapMaterial!");
		return;
	}

	//TODO: update shader variables in material
	m_pShadowMat->SetBones((float*)bones.data(), bones.size());
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);

	ShadowMapMaterial::ShadowGenType id = (pMeshFilter->m_HasAnimations) ? ShadowMapMaterial::Skinned : ShadowMapMaterial::Static;


	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	//Set Inputlayout
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[id]);

	//Set Vertex Buffer
	UINT offset = 0;
	auto vertexBuffer = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[id]);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.pVertexBuffer, &vertexBuffer.VertexStride, &offset);

	//Set Index Buffer
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Primitive Topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//invoke draw call
	auto tech = m_pShadowMat->m_pShadowTechs[id];
	D3DX11_TECHNIQUE_DESC desc;
	tech->GetDesc(&desc);
	for (UINT p = 0; p < desc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	ShadowMapMaterial::ShadowGenType id = (pMeshFilter->m_HasAnimations) ? ShadowMapMaterial::Skinned : ShadowMapMaterial::Static;
	pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[id], m_pShadowMat->m_InputLayoutSizes[id], m_pShadowMat->m_InputLayoutDescriptions[id]);
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
