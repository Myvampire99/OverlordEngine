#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostVignette : public PostProcessingMaterial
{
public:
	PostVignette();
	virtual ~PostVignette() = default;

	PostVignette(const PostVignette& other) = delete;
	PostVignette(PostVignette&& other) noexcept = delete;
	PostVignette& operator=(const PostVignette& other) = delete;
	PostVignette& operator=(PostVignette&& other) noexcept = delete;
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};
