#include "stdafx.h"
#include "UberMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"




ID3DX11EffectVectorVariable* UberMaterial::m_pLightDirectionVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pDiffuseColorVariable = nullptr;

ID3DX11EffectVectorVariable* UberMaterial::m_pSpecularColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseSpecularLevelTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pShininessVariable = nullptr;

ID3DX11EffectVectorVariable* UberMaterial::m_pAmbientColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pAmbientIntensityVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseNormalMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseEnvironmentMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pReflectionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionIndexVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pOpacityVariable = nullptr;
ID3DX11EffectScalarVariable*UberMaterial::m_pUseOpacityMapVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseBlinnVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUsePhongVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pFresnelColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelPowerVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelHardnessVariable = nullptr;

UberMaterial::UberMaterial() : Material(L"./Resources/Effects/UberShader.fx"),
m_pDiffuseTexture(nullptr),
m_pEnvironmentCube(nullptr),
m_pSpecularLevelTexture(nullptr),
m_pOpacityMap(nullptr),
m_pNormalMappingTexture(nullptr)
{}

UberMaterial::~UberMaterial()
{}

void UberMaterial::SetLightDirection(DirectX::XMFLOAT3 direction)
{
	m_LightDirection = direction;
}

void UberMaterial::EnableDiffuseTexture(bool enable)
{
	m_bDiffuseTexture = enable;
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_bDiffuseTexture = true;
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetDiffuseColor(DirectX::XMFLOAT4 color)
{
	m_ColorDiffuse = color;
}

void UberMaterial::SetSpecularColor(DirectX::XMFLOAT4 color)
{
	m_ColorSpecular = color;
}

void UberMaterial::EnableSpecularLevelTexture(bool enable)
{
	m_bSpecularLevelTexture = enable;
}

void UberMaterial::SetSpecularLevelTexture(const std::wstring & assetFile)
{
	m_bSpecularLevelTexture = true;
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetShininess(int shininess)
{
	m_Shininess = shininess;
}

void UberMaterial::SetAmbientColor(DirectX::XMFLOAT4 color)
{
	m_ColorAmbient = color;
}

void UberMaterial::SetAmbientIntensity(float intensity)
{
	m_AmbientIntensity = intensity;
}

void UberMaterial::FlipNormalGreenCHannel(bool flip)
{
	m_bFlipGreenChannel = flip;
}

void UberMaterial::EnableNormalMapping(bool enable)
{
	m_bNormalMapping = enable;
}

void UberMaterial::SetNormalMapTexture(const std::wstring & assetFile)
{
	m_bNormalMapping = true;
	m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableEnvironmentMapping(bool enable)
{
	m_bEnvironmentMapping = enable;
}

void UberMaterial::SetEnvironmentCube(const std::wstring & assetFile)
{
	m_bEnvironmentMapping = true;
	m_pEnvironmentCube = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetReflectionStrength(float strength)
{
	m_ReflectionStrength = strength;
}

void UberMaterial::SetRefractionStrength(float strength)
{
	m_RefractionStrength = strength;
}

void UberMaterial::SetRefractionIndex(float index)
{
	m_RefractionIndex = index;
}

void UberMaterial::SetOpacity(float opacity)
{
	m_Opacity = opacity;
}

void UberMaterial::EnableOpacityMap(bool enable)
{
	m_bOpacityMap = enable;
}

void UberMaterial::SetOpacityTexture(const std::wstring & assetFile)
{
	m_bOpacityMap = true;
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableSpecularBlinn(bool enable)
{
	m_bSpecularBlinn = enable;
}

void UberMaterial::EnableSpecularPhong(bool enable)
{
	m_bSpecularPhong = enable;
}

void UberMaterial::EnableFresnelFaloff(bool enable)
{
	m_bFresnelFaloff = enable;
}

void UberMaterial::SetFresnelColor(DirectX::XMFLOAT4 color)
{
	m_ColorFresnel = color;
}

void UberMaterial::SetFresnelPower(float power)
{
	m_FresnelPower = power;
}

void UberMaterial::SetFresnelMultiplier(float multiplier)
{
	m_FresnelMultiplier = multiplier;
}

void UberMaterial::SetFresnelHardness(float hardness)
{
	m_FresnelHardness = hardness;
}

void UberMaterial::LoadEffectVariables()
{
	//light
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();

	//diffuse
	m_pUseDiffuseTextureVariable = GetEffect()->GetVariableByName("gUseTextureDiffuse")->AsScalar();
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gTextureDiffuse")->AsShaderResource();
	if (!m_pDiffuseSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gTextureDiffuse\' variable not found!");
		m_pDiffuseSRVvariable = nullptr;
	}
	m_pDiffuseColorVariable = GetEffect()->GetVariableByName("gColorDiffuse")->AsVector();

	//specular
	m_pSpecularLevelSRVvariable = GetEffect()->GetVariableByName("gTextureSpecularIntensity")->AsShaderResource();
	if (!m_pSpecularLevelSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gTextureSpecularIntensity\' variable not found!");
		m_pSpecularLevelSRVvariable = nullptr;
	}
	m_pSpecularColorVariable = GetEffect()->GetVariableByName("gColorSpecular")->AsVector();
	m_pUseSpecularLevelTextureVariable = GetEffect()->GetVariableByName("gUseTextureDiffuse")->AsScalar();
	m_pShininessVariable = GetEffect()->GetVariableByName("gShininess")->AsScalar();
	m_pAmbientColorVariable = GetEffect()->GetVariableByName("gColorAmbient")->AsVector();
	m_pAmbientIntensityVariable = GetEffect()->GetVariableByName("gAmbientIntensity")->AsScalar();

	//normal
	m_pFlipGreenChannelVariable = GetEffect()->GetVariableByName("gFlipGreenChannel")->AsScalar();
	m_pUseNormalMappingVariable = GetEffect()->GetVariableByName("gUseTextureNormal")->AsScalar();
	m_pNormalMappingSRVvariable = GetEffect()->GetVariableByName("gTextureNormal")->AsShaderResource();
	if (!m_pNormalMappingSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gTextureNormal\' variable not found!");
		m_pNormalMappingSRVvariable = nullptr;
	}

	//enviroment
	m_pEnvironmentSRVvariable = GetEffect()->GetVariableByName("gCubeEnvironment")->AsShaderResource();
	if (!m_pEnvironmentSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gCubeEnvironment\' variable not found!");
		m_pEnvironmentSRVvariable = nullptr;
	}
	m_pUseEnvironmentMappingVariable = GetEffect()->GetVariableByName("gUseTextureEnvironment")->AsScalar();
	m_pReflectionStrengthVariable = GetEffect()->GetVariableByName("gReflectionStrength")->AsScalar();
	m_pRefractionStrengthVariable = GetEffect()->GetVariableByName("gRefractionStrength")->AsScalar();
	m_pRefractionIndexVariable = GetEffect()->GetVariableByName("gRefractionIndex")->AsScalar();

	m_pOpacityVariable = Material::GetEffect()->GetVariableByName("gOpacityIntensity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gOpacityIntensity"); }

	m_pUseOpacityMapVariable = Material::GetEffect()->GetVariableByName("gUseTextureOpacity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gUseTextureOpacity"); }

	m_pOpacitySRVvariable = Material::GetEffect()->GetVariableByName("gTextureOpacity")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gTextureOpacity"); }

	m_pUseBlinnVariable = Material::GetEffect()->GetVariableByName("gUseSpecularBlinn")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gUseSpecularBlinnn"); }

	m_pUsePhongVariable = Material::GetEffect()->GetVariableByName("gUseSpecularPhong")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gUseSpecularPhong"); }

	m_pUseFresnelFalloffVariable = Material::GetEffect()->GetVariableByName("gUseFresnelFalloff")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gUseFresnelFalloff"); }

	m_pFresnelColorVariable = Material::GetEffect()->GetVariableByName("gColorFresnel")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gColorFresnel"); }

	m_pFresnelPowerVariable = Material::GetEffect()->GetVariableByName("gFresnelPower")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Can't find gFresnelPower"); }
	m_pFresnelMultiplierVariable = Material::GetEffect()->GetVariableByName("gFresnelMultiplier")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFresnelMultiplier!"); }


	m_pFresnelHardnessVariable = Material::GetEffect()->GetVariableByName("gFresnelHardness")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFresnelHardness!"); }


}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	m_pUseDiffuseTextureVariable->SetBool(m_bDiffuseTexture);
	if (m_bDiffuseTexture && m_pDiffuseTexture != nullptr) m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pDiffuseColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorDiffuse));

	m_pSpecularColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorSpecular));
	m_pUseSpecularLevelTextureVariable->SetBool(m_bSpecularLevelTexture);
	if (m_bSpecularLevelTexture && m_pOpacityVariable != nullptr) m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());
	m_pShininessVariable->SetInt(m_Shininess);

	m_pAmbientColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorAmbient));
	m_pAmbientIntensityVariable->SetFloat(m_AmbientIntensity);

	m_pFlipGreenChannelVariable->SetBool(m_bFlipGreenChannel);
	m_pUseNormalMappingVariable->SetBool(m_bNormalMapping);
	if (m_bNormalMapping && m_pNormalMappingTexture != nullptr) m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());

	m_pUseEnvironmentMappingVariable->SetBool(m_bEnvironmentMapping);
	if (m_bEnvironmentMapping && m_pEnvironmentCube != nullptr) m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCube->GetShaderResourceView());
	m_pReflectionStrengthVariable->SetFloat(m_ReflectionStrength);
	m_pRefractionStrengthVariable->SetFloat(m_RefractionStrength);
	m_pRefractionIndexVariable->SetFloat(m_RefractionIndex);

	m_pOpacityVariable->SetFloat(m_Opacity);
	m_pUseOpacityMapVariable->SetBool(m_bOpacityMap);
	if (m_bOpacityMap && m_pOpacityMap != nullptr) m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());

	m_pUseBlinnVariable->SetBool(m_bSpecularBlinn);
	m_pUsePhongVariable->SetBool(m_bSpecularPhong);

	m_pUseFresnelFalloffVariable->SetBool(m_bFresnelFaloff);
	m_pFresnelColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorFresnel));
	m_pFresnelPowerVariable->SetFloat(m_FresnelPower);
	m_pFresnelMultiplierVariable->SetFloat(m_FresnelMultiplier);
	m_pFresnelHardnessVariable->SetFloat(m_FresnelHardness);
}