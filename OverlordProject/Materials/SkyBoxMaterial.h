#pragma once
#include "Material.h"
class TextureData;

class SkyBoxMaterial : public Material
{
public:
	SkyBoxMaterial();
	~SkyBoxMaterial() = default;

	void SetSkyBoxMat(std::wstring mat);
protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SkyBoxMaterial(const SkyBoxMaterial &obj);
	SkyBoxMaterial& operator=(const SkyBoxMaterial& obj);

	TextureData* m_pDiffuseTexture;
};

