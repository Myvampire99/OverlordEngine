#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	float elapsed = gameContext.pGameTime->GetElapsed();
	//TODO: See Lab9_2
	if (!m_IsActive)
		return;

	m_CurrentEnergy -= elapsed;
	if (m_CurrentEnergy < 0.f) {
		m_IsActive = false;
		return;
	}

	auto vel = m_EmitterSettings.Velocity;
	m_VertexInfo.Position.x +=  vel.x*elapsed;
	m_VertexInfo.Position.y += vel.y*elapsed;
	m_VertexInfo.Position.z +=  vel.z*elapsed;

	m_VertexInfo.Color = m_EmitterSettings.Color;
	auto particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2.f;

	if (m_SizeGrow < 1) {
		m_VertexInfo.Size = (1.f - ((1.f - m_SizeGrow)*(1.f - particleLifePercent)))*m_InitSize;
	}
	else {
		m_VertexInfo.Size = ((m_SizeGrow - m_InitSize)*(1.f - particleLifePercent) + 1)*m_InitSize;
	}
}

void Particle::Init(XMFLOAT3 initPosition)
{

	m_IsActive = true;

	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy,m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	DirectX::XMVECTOR RandomVector{1.f,0.f,0.f};
	auto randomRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(randF(-XM_PI,XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));

	DirectX::XMVector3TransformNormal(RandomVector, randomRotationMatrix);
	auto randDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	XMFLOAT3 float3;
	XMStoreFloat3(&float3, RandomVector);

	m_VertexInfo.Position.x = float3.x * randDistance + initPosition.x;
	m_VertexInfo.Position.y = float3.y * randDistance + initPosition.y;
	m_VertexInfo.Position.z = float3.z * randDistance + initPosition.z;

	m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_VertexInfo.Size = m_InitSize;

	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);

}
