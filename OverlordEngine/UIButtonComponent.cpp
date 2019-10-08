#include "stdafx.h"
#include "UIButtonComponent.h"
#include "SpriteComponent.h"
#include "TextRenderer.h"
#include "ContentManager.h"
#include "TransformComponent.h"
#include "SpriteFont.h"
#include "TextureData.h"
#include "GameObject.h"
#include "SoundManager.h"


UIButtonComponent::UIButtonComponent(std::wstring text, SpriteComponent *spriteComp, std::wstring button, std::wstring buttonPressed)
	:m_Text{text}
	, m_PathButton{ button }
	, m_PathButtonPressed{ buttonPressed }
	, m_pSpriteComponent{ spriteComp }
	, offsetX{45}
	, offsetY{ 10 }
	, m_NeedUpdate{false}
	, m_Pressed{-1}
	, m_Released{false}
	, m_Disabled{false}
	, m_Hovering{false}
	, m_pHoverSound{nullptr}
	, m_PlaySound{true}
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");
}


UIButtonComponent::~UIButtonComponent()
{}

void UIButtonComponent::Initialize(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);

	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/Mouse.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pHoverSound);
}

void UIButtonComponent::Update(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);
	
	if (!m_Disabled) {

		auto float2 = m_pSpriteComponent->GetTexture()->GetDimension();
		auto pos = m_pGameObject->GetTransform()->GetPosition();
		auto mouse = gameContext.pInput->GetMousePosition();
		auto scale = m_pGameObject->GetTransform()->GetScale();

		m_Hovering = false;

		if ((mouse.x > pos.x && mouse.x < pos.x + float2.x*scale.x) && (mouse.y > pos.y && mouse.y < pos.y + float2.y*scale.y)) {
			m_pSpriteComponent->SetTexture(m_PathButtonPressed);
			m_NeedUpdate = true;
			m_Hovering = true;

			if (m_PlaySound) {
				SoundManager::GetInstance()->GetSystem()->playSound(m_pHoverSound, 0, false, 0);
				m_PlaySound = false;
			}
			

			if (gameContext.pInput->IsMouseButtonDown(0x01)) {
				m_Released = true;
				

			}
			else {
				if (m_Released)
					m_Pressed = m_IfPressed;
			}

		}
		else if (m_NeedUpdate) {
			m_pSpriteComponent->SetTexture(m_PathButton);
			m_PlaySound = true;
		}

	}
}

void UIButtonComponent::Draw(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);

	

}

void UIButtonComponent::PostDraw(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);
	auto pos = m_pGameObject->GetTransform()->GetPosition();
	if (!m_Disabled) {
		TextRenderer::GetInstance()->DrawText(m_pFont, m_Text, DirectX::XMFLOAT2(pos.x + offsetX, pos.y + offsetY));
	}
	else {
		TextRenderer::GetInstance()->DrawText(m_pFont, m_Text, DirectX::XMFLOAT2(pos.x + offsetX, pos.y + offsetY), DirectX::XMFLOAT4(DirectX::Colors::Red));
	}
}

void UIButtonComponent::ChangeText(std::wstring text) {
	m_Text = text;
}

int UIButtonComponent::GetPressed() {
	return m_Pressed;
}

void UIButtonComponent::SetPressed(int pressed, int notPressed) {
	m_IfPressed = pressed;
	m_Pressed = notPressed;
}