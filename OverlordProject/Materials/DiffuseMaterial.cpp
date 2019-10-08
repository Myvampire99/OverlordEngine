#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"

DiffuseMaterial::DiffuseMaterial()
	:Material(L"Resources/Effects/PosNormTex3D.fx")
	, m_pDiffuseTexture{nullptr}
{}


DiffuseMaterial::~DiffuseMaterial()
{
	
}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring & assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

void DiffuseMaterial::UpdateEffectVariables(const GameContext & gameContext, ModelComponent * pModelComponent)
{
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
}
