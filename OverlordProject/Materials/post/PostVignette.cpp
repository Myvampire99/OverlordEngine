#include "stdafx.h"
#include "PostVignette.h"
#include "RenderTarget.h"

PostVignette::PostVignette()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Vignette.fx", 3),
	m_pTextureMapVariabele(nullptr)
{
}

void PostVignette::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!


	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();

}

void PostVignette::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}