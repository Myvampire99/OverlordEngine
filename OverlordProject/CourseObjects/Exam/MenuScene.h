#pragma once
#include "GameScene.h"
class MenuScene : public GameScene
{
public:
	MenuScene();
	~MenuScene();


	MenuScene(const MenuScene& other) = delete;
	MenuScene(MenuScene&& other) noexcept = delete;
	MenuScene& operator=(const MenuScene& other) = delete;
	MenuScene& operator=(MenuScene&& other) noexcept = delete;


	GameObject* m_BackGround;
	GameObject* m_Start;
	GameObject* m_Stop;

protected:
	void Initialize();
	void Update();
	void Draw();
};

