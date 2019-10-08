#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount, DirectX::XMFLOAT3 offset):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pTextureCamPos(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile)),
	m_Pause{false},
	m_Offset{ offset }
{
	//TODO: See Lab9_2
	//m_Particles.resize(m_ParticleCount, new Particle(m_Settings));
	//std::fill(m_Particles.begin(), m_Particles.end(),new Particle(m_Settings));

	for (int i{}; i < particleCount; ++i) {
		m_Particles.push_back(new Particle(m_Settings));
	}

}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (auto p : m_Particles)
		delete p;

	m_pInputLayout->Release();
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);

	
	auto mod = m_Offset;
	mod.x += GetTransform()->GetPosition().x;
	mod.y += GetTransform()->GetPosition().y;
	mod.z += GetTransform()->GetPosition().z;

	for (auto p : m_Particles)
		p->Init(GetTransform()->GetPosition());

}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	m_pWvpVariable = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
	if (!m_pWvpVariable->IsValid())Logger::LogError(L"ParticleEmitterComponent::LoadEffect > error gWorldViewProj");
	m_pViewInverseVariable = m_pEffect->GetVariableBySemantic("ViewInverse")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())Logger::LogError(L"ParticleEmitterComponent::LoadEffect > error gViewInverse");
	m_pTextureVariable = m_pEffect->GetVariableBySemantic("ParticleTexture")->AsShaderResource();
	if (!m_pTextureVariable->IsValid())Logger::LogError(L"ParticleEmitterComponent::LoadEffect > error gParticleTexture");

	m_pTextureCamPos = m_pEffect->GetVariableByName("CameraPosition")->AsVector();
	if (!m_pTextureVariable->IsValid())Logger::LogError(L"ParticleEmitterComponent::LoadEffect > error gTextureSize");

	UINT size =  0;
	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, size);




}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(ParticleVertex)* m_ParticleCount;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	const auto hr = gameContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer);
	if (Logger::LogHResult(hr, L"ParticleEmitterComponent::CreateVertexBuffer > Vertexbuffer creation failed!"))
		return;

}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	if (!m_Pause) {
		float particleInterval{ (m_Settings.MinEnergy + m_Settings.MaxEnergy) / 2.f };
		particleInterval = particleInterval / m_ParticleCount;

		particleInterval = (m_Settings.MaxEnergy - m_Settings.MinEnergy) / m_ParticleCount;
		particleInterval = 0;


		m_LastParticleInit += gameContext.pGameTime->GetElapsed();


		//BUFFER MAPPING CODE [PARTIAL :)]
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//D3D11_MAP_WRITE_DISCARD
		ParticleVertex* pBuffer = (ParticleVertex*)mappedResource.pData;

		m_ActiveParticles = 0;
		for (auto particle : m_Particles) {
			particle->Update(gameContext);
			if (particle->IsActive()) {
				pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
				m_ActiveParticles++;
			}
			else if (m_LastParticleInit >= particleInterval) {
				particle->Init(GetTransform()->GetPosition());
				pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
				m_ActiveParticles++;
				m_LastParticleInit = 0;
			}
		}
		gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
	}
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{



	if (!m_Pause) {

		auto test = GetTransform();
		UNREFERENCED_PARAMETER(test);



		//Render pipeline
		gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		unsigned int stride = sizeof(ParticleVertex);
		unsigned int offset = 0;
		gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		//gameContext.pDeviceContext->IASetIndexBuffer(, DXGI_FORMAT_R32_UINT, 0);
		gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);




	/*	auto world = XMLoadFloat4x4(&GetTransform()->GetWorldWithoutRotation());
		auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
		auto wvp = world * viewProjection;
		m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&viewProjection));*/
		{
			auto world = XMLoadFloat4x4(&GetTransform()->GetWorld());
			auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());//GetViewProjection
			auto wvp = world * viewProjection;
			auto trans = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
			m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&trans));
		}

		auto viewProjection2 = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());//GetViewInverse

		auto world = XMLoadFloat4x4(&GetTransform()->GetWorldWithoutRotation());
		auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetProjection());//GetProjection
		auto wvp = world * viewProjection;

		m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&viewProjection2));



		m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

		DirectX::XMFLOAT3 CamPos({ gameContext.pCamera->GetTransform()->GetPosition() });
		m_pTextureCamPos->SetFloatVector(reinterpret_cast<float*>(&CamPos));









		D3DX11_TECHNIQUE_DESC techDesc;
		m_pDefaultTechnique->GetDesc(&techDesc);
		for (unsigned int j = 0; j < techDesc.Passes; ++j)
		{
			m_pDefaultTechnique->GetPassByIndex(j)->Apply(0, gameContext.pDeviceContext);
			gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
		}
	}
}
