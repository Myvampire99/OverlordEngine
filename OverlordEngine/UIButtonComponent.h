#pragma once
#include "BaseComponent.h"

class SpriteFont;
class SpriteComponent;
class UIButtonComponent : public BaseComponent
{
public:
	UIButtonComponent(std::wstring text,SpriteComponent *spriteComp, std::wstring button = L"Resources/Textures/UIButton.png", std::wstring buttonPressed = L"Resources/Textures/UIButton_Selected.png");
	~UIButtonComponent();

	void Initialize(const GameContext& gameContext);
	void Update(const GameContext& gameContext);
	void Draw(const GameContext& gameContext);
	void PostDraw(const GameContext& gameContext);

	void ChangeText(std::wstring text);
	int GetPressed();
	void SetPressed(int pressed,int notPressed = -1);

	void SetDisabled(bool disabled) { m_Disabled = disabled; };
	bool IsDisabled() { return m_Disabled; };
	bool IsHovering() { return m_Hovering; };

	int GetStoredNumber() { return m_IfPressed; };
	void ResetPressed() {
		m_Pressed = -1; m_Released
			= false;
	};
private:
	std::wstring m_PathButton;
	std::wstring m_PathButtonPressed;
	std::wstring m_Text;

	SpriteFont* m_pFont;
	SpriteComponent* m_pSpriteComponent;

	float offsetX, offsetY;
	bool m_NeedUpdate;
	int m_Pressed;
	int m_IfPressed;

	bool m_Released;
	bool m_Disabled;
	bool m_Hovering;
	bool m_PlaySound;

	FMOD::Sound* m_pHoverSound;
};

