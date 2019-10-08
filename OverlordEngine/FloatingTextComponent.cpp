#include "stdafx.h"
#include "FloatingTextComponent.h"
#include "ContentManager.h"
#include "EffectHelper.h"
#include "OverlordGame.h"
#include "SpriteFont.h"
#include "TextureData.h"
#include "GameObject.h"
#include "TransformComponent.h"

FloatingTextComponent::FloatingTextComponent() :
	m_BufferSize(500),
	m_InputLayoutSize(0),
	m_NumCharacters(0),
	m_Transform(DirectX::XMFLOAT4X4()),
	m_pEffect(nullptr),
	m_pTechnique(nullptr),
	m_pTransfromMatrixV(nullptr),
	m_pTextureSizeV(nullptr),
	m_pTextureSRV(nullptr),
	m_pInputLayout(nullptr),
	m_pVertexBuffer(nullptr),
	m_SpriteFonts(std::vector<SpriteFont*>())
	, m_NeedUpdate{false}
{
}

FloatingTextComponent::~FloatingTextComponent()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

//
void FloatingTextComponent::Initialize(const GameContext& gameContext)
{
	//ID3D11Device
	auto pDevice = gameContext.pDevice;

	//Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/FloatingText.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	m_pTransfromMatrixV = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pTransfromMatrixV->IsValid())
	{
		Logger::LogError(L"SpriteRenderer::Initialize() > Shader variable \'gTransform\' not valid!");
		return;
	}

	m_pTextureSizeV = m_pEffect->GetVariableByName("gTextureSize")->AsVector();
	if (!m_pTextureSizeV->IsValid())
	{
		Logger::LogError(L"SpriteRenderer::Initialize() > Shader variable \'gTextureSize\' not valid!");
		return;
	}

	m_pTextureSRV = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	if (!m_pTextureSRV->IsValid())
	{
		Logger::LogError(L"SpriteRenderer::Initialize() > Shader variable \'gSpriteTexture\' not valid!");
		return;
	}

	m_pViewInverse = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pViewInverse->IsValid())
	{
		Logger::LogError(L"SpriteRenderer::Initialize() > Shader variable \'gViewInverse\' not valid!");
		return;
	}

	EffectHelper::BuildInputLayout(pDevice, m_pTechnique, &m_pInputLayout, m_InputLayoutSize);

	//Transform Matrix
	const auto settings = OverlordGame::GetGameSettings();
	const float scaleX = (settings.Window.Width > 0) ? 2.0f / settings.Window.Width : 0;
	const float scaleY = (settings.Window.Height > 0) ? 2.0f / settings.Window.Height : 0;

	m_Transform._11 = scaleX;
	m_Transform._12 = 0;
	m_Transform._13 = 0;
	m_Transform._14 = 0;
	m_Transform._21 = 0;
	m_Transform._22 = -scaleY;
	m_Transform._23 = 0;
	m_Transform._24 = 0;
	m_Transform._31 = 0;
	m_Transform._32 = 0;
	m_Transform._33 = 1;
	m_Transform._34 = 0;
	m_Transform._41 = -1;
	m_Transform._42 = 1;
	m_Transform._43 = 0;
	m_Transform._44 = 1;
}

void FloatingTextComponent::DrawText(SpriteFont* pFont, const std::wstring& text, DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT4 color)
{
	m_SpriteFonts.clear();

	UNREFERENCED_PARAMETER(position);
	

	m_NumCharacters += wcslen(text.c_str());
	pFont->AddToTextCache(TextCache(text, DirectX::XMFLOAT3{ m_pGameObject->GetTransform()->GetPosition().x + position.x,m_pGameObject->GetTransform()->GetPosition().y + position.y,m_pGameObject->GetTransform()->GetPosition().z + position.z }, color));
	if (!pFont->IsAddedToRenderer())
	{
		m_SpriteFonts.push_back(pFont);
		pFont->SetAddedToRenderer(true);
	}
	m_NeedUpdate = true;
}

void FloatingTextComponent::Draw(const GameContext& gameContext)  {

	UNREFERENCED_PARAMETER(gameContext);



}

void FloatingTextComponent::PostDraw(const GameContext& gameContext)
{
	

	if (m_SpriteFonts.empty())
		return;

	if (m_NeedUpdate) {
		UpdateBuffer(gameContext);
		m_NeedUpdate = false;
	}


	//Set Render Pipeline
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	unsigned int stride = sizeof(TextVertex);
	unsigned int offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	for each (SpriteFont* pFont in m_SpriteFonts)
	{
		//Set Texture
		m_pTextureSRV->SetResource(pFont->GetTexture()->GetShaderResourceView());

		//Set Texture Size
		auto texSize = pFont->GetTexture()->GetDimension();
		m_pTextureSizeV->SetFloatVector(reinterpret_cast<float*>(&texSize));

		{
			auto backup = GetTransform()->GetScale();
			GetTransform()->Scale(0.1f,0.1f,0.1f);
			auto world = XMLoadFloat4x4(&GetTransform()->GetWorldWithoutRotation());
			auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
			auto wvp = world * viewProjection;
			auto trans = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
			m_pTransfromMatrixV->SetMatrix(reinterpret_cast<float*>(&wvp));
			GetTransform()->Scale(backup);
		}




		//Set viewInverse//
		auto backup = GetTransform()->GetScale();
		GetTransform()->Scale(0.1f, 0.1f, 0.1f);
		auto viewProjection2 = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());//GetViewInverse

		auto world = XMLoadFloat4x4(&GetTransform()->GetWorldWithoutRotation());
		auto viewProjection = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
		auto wvp = world * viewProjection;

		m_pViewInverse->SetMatrix(reinterpret_cast<float*>(&wvp));
		GetTransform()->Scale(backup);



		D3DX11_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc(&techDesc);
		for (unsigned int i = 0; i < techDesc.Passes; ++i)
		{
			m_pTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
			gameContext.pDeviceContext->Draw(pFont->GetBufferSize(), pFont->GetBufferStart());
		}

		//pFont->SetAddedToRenderer(false);
	}

	//m_SpriteFonts.clear();
}


void FloatingTextComponent::Update(const GameContext& gameContext)  {
	UNREFERENCED_PARAMETER(gameContext);
}

void FloatingTextComponent::UpdateBuffer(const GameContext& gameContext)
{
	if (!m_pVertexBuffer || m_NumCharacters > m_BufferSize)
	{
		//Release the buffer if it exists
		SafeRelease(m_pVertexBuffer);

		//Set buffersize if needed
		if (m_NumCharacters > m_BufferSize)
			m_BufferSize = m_NumCharacters;

		//Create a new buffer
		D3D11_BUFFER_DESC buffDesc;
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.ByteWidth = sizeof(TextVertex) * m_NumCharacters;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;

		auto hr = gameContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer);
		if (Logger::LogHResult(hr, L"TextRenderer::UpdateBuffer"))
			return;
	}

	//Fill Buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	int bufferPosition = 0;
	for each (SpriteFont* pFont in m_SpriteFonts)
	{
		const auto& cache = pFont->GetTextCache();
		pFont->SetBufferStart(bufferPosition);
		for (unsigned int i = 0; i < cache.size(); ++i)
		{
			CreateTextVertices(pFont, cache[i], static_cast<TextVertex*>(mappedResource.pData), bufferPosition);
		}

		pFont->SetBufferSize(bufferPosition - pFont->GetBufferStart() );
		pFont->ClearCache();
	}
	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);

	m_NumCharacters = 0;
}

void FloatingTextComponent::CreateTextVertices(const SpriteFont* pFont, const TextCache& info, TextVertex* pBuffer,
	int& bufferPosition)
{

	float m_Scale{1.f};


	int totalAdvanceX = 0;
	for (wchar_t charId : info.Text)
	{
		if (SpriteFont::IsCharValid(charId) && pFont->GetMetric(charId).IsValid)
		
		{
			const auto metric = pFont->GetMetric(charId);

			if (charId == ' ')
			{
				totalAdvanceX += int(metric.AdvanceX*m_Scale);
				continue;
			}

			TextVertex vertexText;
			vertexText.Position.x = info.Position.x + totalAdvanceX + metric.OffsetX*m_Scale;
			vertexText.Position.y = info.Position.y + metric.OffsetY*m_Scale;
			vertexText.Position.z = info.Position.z;
			vertexText.Color = info.Color;
			vertexText.TexCoord = metric.TexCoord;
			vertexText.CharacterDimension = DirectX::XMFLOAT2(metric.Width, metric.Height);
			vertexText.ChannelId = metric.Channel;

			pBuffer[bufferPosition] = vertexText;

			++bufferPosition;
			totalAdvanceX += int(metric.AdvanceX*m_Scale);
		}
		else
		{
			Logger::LogFormat(
				Warning, L"TextRenderer::CreateTextVertices > Char not supported for current font.\nCHARACTER: %c (%i)\nFONT: %s",
				charId, static_cast<int>(charId), pFont->GetFontName().c_str());
		}
	}
}
