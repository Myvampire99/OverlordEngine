#include "stdafx.h"
#include "MenuScene.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "UIButtonComponent.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include "ContentManager.h"
#include "SpriteFont.h"


MenuScene::MenuScene()
	:GameScene(L"MenuScene")
{
	m_BackGround = new GameObject();
	m_Start = new GameObject();
	m_Stop = new GameObject();
}


MenuScene::~MenuScene()
{
}

void MenuScene::Initialize() {
	m_BackGround->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2019_MainMenu_Template.png"));
	AddChild(m_BackGround);

	auto comp = new SpriteComponent(L"./Resources/Textures/UIButton.png");
	m_Start->AddComponent(comp);
	m_Start->AddComponent(new UIButtonComponent(L"Start", comp));
	m_Start->GetTransform()->Scale(0.28f, 0.28f, 1.f);
	m_Start->GetTransform()->Translate(0.f,0.f,0.f);
	m_Start->GetComponent<UIButtonComponent>()->SetPressed(0);

	comp = new SpriteComponent(L"./Resources/Textures/UIButton.png");
	m_Stop->AddComponent(comp);
	m_Stop->AddComponent(new UIButtonComponent(L"Stop", comp));
	m_Stop->GetTransform()->Scale(0.28f, 0.28f, 1.f);
	m_Stop->GetTransform()->Translate(0.f, 50.f, 0.f);
	m_Stop->GetComponent<UIButtonComponent>()->SetPressed(1);

	AddChild(m_Start);
	AddChild(m_Stop);
}
void MenuScene::Update() {


	if (m_Start->GetComponent<UIButtonComponent>()->GetPressed() == 0) {
		SceneManager::GetInstance()->SetActiveGameScene(L"ExamScene");
		m_Start->GetComponent<UIButtonComponent>()->ResetPressed();
	}

	if (m_Stop->GetComponent<UIButtonComponent>()->GetPressed() == 1) {
		PostQuitMessage(0);
	}

	auto m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_20.fnt");
	TextRenderer::GetInstance()->DrawText(m_pFont, L"-R- to Go Back To The Main Menu", DirectX::XMFLOAT2(500, 20), DirectX::XMFLOAT4(DirectX::Colors::White));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"-O- Enable Debug Info Characters", DirectX::XMFLOAT2(500, 40), DirectX::XMFLOAT4(DirectX::Colors::White));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"-Q- Debug Info, Next Character", DirectX::XMFLOAT2(500, 60), DirectX::XMFLOAT4(DirectX::Colors::White));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"-E- Debug Info, Prev Character", DirectX::XMFLOAT2(500, 80), DirectX::XMFLOAT4(DirectX::Colors::White));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Please keep the window at it's current size!", DirectX::XMFLOAT2(500, 100), DirectX::XMFLOAT4(DirectX::Colors::Yellow));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"My Button Hitboxes Tend to displace themselves...", DirectX::XMFLOAT2(500, 120), DirectX::XMFLOAT4(DirectX::Colors::Yellow));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Sorry for the inconvenience", DirectX::XMFLOAT2(500, 140), DirectX::XMFLOAT4(DirectX::Colors::Yellow));
}
void MenuScene::Draw() {

}