//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkinnedDiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransforms = nullptr;
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;
}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
	m_pBoneTransforms = GetEffect()->GetVariableByName("gBones")->AsMatrix();
}

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());

	auto lvp = gameContext.pShadowMapper->GetLightVP();
	auto viewProjection = XMLoadFloat4x4(&lvp);

	auto wvp = world * viewProjection;
	m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&wvp));

	auto bones = pModelComponent->GetAnimator()->GetBoneTransforms();
	m_pBoneTransforms->SetMatrixArray((float*)bones.data(), 0, bones.size());
}