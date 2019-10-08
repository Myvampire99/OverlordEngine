#include "stdafx.h"
#include "SkyBoxMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"

ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable = nullptr;

SkyBoxMaterial::SkyBoxMaterial() :
	Material(L"./Resources/Effects/SkyBox.fx")
	, m_pDiffuseTexture(nullptr)
{}


void SkyBoxMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("m_CubeMap")->AsShaderResource();
}

void SkyBoxMaterial::SetSkyBoxMat(std::wstring mat) {
	m_pDiffuseTexture = ContentManager::Load<TextureData>(mat);
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
}