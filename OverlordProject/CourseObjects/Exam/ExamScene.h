#pragma once
#include "GameScene.h"
#include "CharacterData.h"

class SoundManager;


class ExamScene :
	public GameScene
{
public:
	ExamScene();
	~ExamScene();

	ExamScene(const ExamScene& other) = delete;
	ExamScene(ExamScene&& other) noexcept = delete;
	ExamScene& operator=(const ExamScene& other) = delete;
	ExamScene& operator=(ExamScene&& other) noexcept = delete;

protected:
	void Initialize();
	void Update();
	void Draw();

private:
	std::vector<GameObject*> m_FriendlyTeam;
	std::vector<GameObject*> m_EnemyTeam;
	std::vector<GameObject*> m_UIButtons;

	GameObject* test;

	void CreateButton(std::wstring text, DirectX::XMFLOAT3 position, int numberIfPressed, bool disabled = false);
	void CreateNextUI();
	void DeletePrevUI();

	void CreateCharacter(std::wstring name,std::wstring path,int matId,int scale = 1.f,bool IsEnemy = false);
	void CreateUIChooseAttack();
	void CreateUIChooseEnemy();
	bool CheckIfPressed();

	void CompleteAction();

	int m_CurrentPlayer;
	bool m_NeedUpdate;
	bool m_ForceUpdate;

	int m_GetPressedButtonNr;
	int m_Target;
	int m_ActionID;

	enum CurrentBattleState {
		BS_ChooseEnemy,
		BS_ChooseAttack,
		BS_EnemyBattle,
		BS_NextCharacter,
		BS_Attacking,
		BS_Defeated,
		BS_Won,
		BS_ENDGAME
	};

	bool m_ENDGAME = false;
	
	CurrentBattleState m_BState;

	float m_HeightMargin = 50.f;
	float m_WindowMargin = 15.f;
	float m_NameMargin = 120.f;
	float m_LeftMarge = 15.f;

	float m_CharacterSpace = 4.f;
	float m_CharacterTeamSpace = 10.5f;
	

	void DebugDrawInfo(GameObject* character);
	int debugCounter;

	void DrawCharacterInfo();
	void CurrentPlayerInfoDraw();
	void DrawActionsInfo();
	void AddActions();

	int m_BackNumber = -3;
	GameObject* m_SkyBox;

	float m_RotationSky;

	void FancySkyRotation();

	FMOD::Sound* m_pMenuSong;
	SoundManager* m_pSoundManager;
	bool m_IniUpdate;

	std::pair<CharacterData::Action,GameObject*> AIActions(GameObject* character,std::vector<GameObject*> enemies,int &TurnsToBuff);
	std::vector<int> m_EnemyTeamBuffTurns{3};
	void ChooseAttackAi();

	bool m_ShowDebugCharacter;
	
};

