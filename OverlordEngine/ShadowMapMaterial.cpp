//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{}
	

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (!m_IsInitialized)
	{
		//TODO: initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");

		m_pShadowTechs[Static] = m_pShadowEffect->GetTechniqueByName("GenerateShadows");
		m_pShadowTechs[Skinned] = m_pShadowEffect->GetTechniqueByName("GenerateShadows_Skinned");

		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();
		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();

		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[Static], &m_pInputLayouts[Static], m_InputLayoutDescriptions[Static],
			m_InputLayoutSizes[Static], m_InputLayoutIds[Static]);

		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[Skinned], &m_pInputLayouts[Skinned], m_InputLayoutDescriptions[Skinned],
			m_InputLayoutSizes[Skinned], m_InputLayoutIds[Skinned]);
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	//TODO: set the correct shader variable
	m_pLightVPMatrixVariable->SetMatrix(reinterpret_cast<float*>(&lightVP));
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	//TODO: set the correct shader variable
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	m_pBoneTransforms->SetMatrixArray(pData, 0, count);
	//TODO: set the correct shader variable
}
