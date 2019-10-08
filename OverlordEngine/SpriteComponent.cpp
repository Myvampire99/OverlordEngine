#include "stdafx.h"

#include "SpriteComponent.h"
 #include <utility>

#include "GameObject.h"
#include "TextureData.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TransformComponent.h"

SpriteComponent::SpriteComponent(std::wstring spriteAsset, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color):
	m_pTexture(nullptr),
	m_SpriteAsset(std::move(spriteAsset)),
	m_Pivot(pivot),
	m_Color(color)
{
}

void SpriteComponent::Initialize(const GameContext& )
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

TextureData* SpriteComponent::GetTexture() {
	return m_pTexture;
}

void SpriteComponent::Update(const GameContext& )
{
}

void SpriteComponent::Draw(const GameContext& )
{
	if (!m_pTexture)
		return;

	//TODO: Here you need to draw the SpriteComponent using the Draw of the sprite renderer
	// The sprite renderer is a singleton
	// you will need to position, the rotation and the scale
	// You can use the QuaternionToEuler function to help you with the z rotation 
	////SpriteVertex temp{};
	////temp.Color = m_Color;

	////temp.TransformData.x = m_pGameObject->GetTransform()->GetPosition().x; 
	////temp.TransformData.y = m_pGameObject->GetTransform()->GetPosition().y;
	////temp.TransformData.z =  m_pGameObject->GetTransform()->GetPosition().z;
	////temp.TransformData.w = QuaternionToEuler({ m_pGameObject->GetTransform()->GetWorldPosition().x,m_pGameObject->GetTransform()->GetWorldPosition().y
	////,m_pGameObject->GetTransform()->GetPosition().z,m_pGameObject->GetTransform()->GetRotation().z }).z;
	////temp.TransformData.w = 0;// m_pGameObject->GetTransform()->GetRotation().z;//QuaternionToEuler({0,0,0,0});

	////temp.TransformData2.z = 1; m_pGameObject->GetTransform()->GetScale().x;
	////temp.TransformData2.w = 1; m_pGameObject->GetTransform()->GetScale().y;

	////SpriteRenderer::GetInstance()->Draw(m_pTexture, DirectX::XMFLOAT2{ temp.TransformData.x ,temp.TransformData.y }, temp.Color, m_Pivot, DirectX::XMFLOAT2{ temp.TransformData2.z, temp.TransformData2.w }, temp.TransformData.w, m_pGameObject->GetTransform()->GetRotation().z);


	SpriteRenderer::GetInstance()->Draw(m_pTexture, DirectX::XMFLOAT2{ GetGameObject()->GetTransform()->GetPosition().x,GetGameObject()->GetTransform()->GetPosition().y } , m_Color, m_Pivot, DirectX::XMFLOAT2{ GetGameObject()->GetTransform()->GetScale().x, GetGameObject()->GetTransform()->GetScale().y }, GetGameObject()->GetTransform()->GetRotation().z, GetGameObject()->GetTransform()->GetPosition().z);
}
