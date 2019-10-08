#pragma once
#include "Singleton.h"

class OverlordGame;
class SpriteFont;
class GameScene;
struct TextCache;

class FloatingTextComponent : public BaseComponent
{
public:
#undef DrawText
	void DrawText(SpriteFont* pFont, const std::wstring& text, DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));

	FloatingTextComponent(const FloatingTextComponent& other) = delete;
	FloatingTextComponent(FloatingTextComponent&& other) noexcept = delete;
	FloatingTextComponent& operator=(const FloatingTextComponent& other) = delete;
	FloatingTextComponent& operator=(FloatingTextComponent&& other) noexcept = delete;

	FloatingTextComponent();
	~FloatingTextComponent();

private:

	struct TextVertex
	{
		unsigned int ChannelId;
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 TexCoord;
		DirectX::XMFLOAT2 CharacterDimension;
	};


	friend class Singleton<FloatingTextComponent>;
	friend class GameScene;
	friend class OverlordGame;



	void Update(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void PostDraw(const GameContext& gameContext) override;
	void UpdateBuffer(const GameContext& gameContext);
	static void CreateTextVertices(const SpriteFont* pFont, const TextCache& info, TextVertex* pBuffer, int& bufferPosition);

	unsigned int m_BufferSize, m_InputLayoutSize, m_NumCharacters;
	DirectX::XMFLOAT4X4 m_Transform;
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pTransfromMatrixV;
	ID3DX11EffectMatrixVariable* m_pViewInverse;
	ID3DX11EffectVectorVariable* m_pTextureSizeV;
	ID3DX11EffectShaderResourceVariable* m_pTextureSRV;

	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;

	std::vector<SpriteFont*> m_SpriteFonts;
	bool m_NeedUpdate;
};
