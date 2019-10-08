#pragma once
#include "BaseComponent.h"

class ParticleEmitterComponent;
class FloatingTextComponent;
class SpriteFont;
class CharacterData : public BaseComponent
{

public:
	struct ConditionBuff {
		std::wstring name{ L"CDB..." };
		int turnsRemaining{0};
		int hpPerTurn{0};//Heal or Damage
		int mpPerTurn{0};
		float defense{1.f};//Weaken or Strengthen, %
		float damageMod{1.f};// same %
	};

	struct Action {
		int damage{ 0 };
		int mpDamage{0};
		int mpCost{0};
		std::wstring name{L"..."};
		std::vector<ConditionBuff> Enemy_CondBuff{};
		std::vector<ConditionBuff> Self_CondBuff{};
		bool Static{ false };
	};

	CharacterData();
	~CharacterData();

	void Draw(const GameContext& gameContext);
	void Initialize(const GameContext& gameContext);
	void Update(const GameContext& gameContext);

	void DoAction(GameObject *GOenemy,int actionID);
	void StartAttackingAnimation(bool attack);
	void StartSelfAnimation(bool attack);
	bool IsReadyWithAction();

	void DoActionOnMe(Action &action);

	std::vector<ConditionBuff> GetConditionBuffs();

	void NotifyStartTurn();
	bool HasEnoughMP(const Action &action);

private:

	struct CharacterInfo{
		int hp{100};
		int mp{100};
		float defenseMod{ 1.f };
		float attackMod{1.f};
		std::wstring name;
	};
	CharacterInfo m_CharacterInfo;
	std::vector<Action> m_Actions;
	int m_MaxHp;
	int m_MaxMp;
	std::vector<ConditionBuff> m_CurrentCondBuffs;


public:
	CharacterInfo& GetCharacterInfo();
	std::vector<CharacterData::Action>& GetActions();


	//Private: Non Gameplay things
	bool ReadyWithAttacking();
	bool m_StartAttacking;
	bool m_StartActionAnimation;
	DirectX::XMFLOAT3 m_TargetPosition;
	float m_WalkingSpeed;
	enum AnimationState {
		Idle = 0, Hit = 1,dying = 2, magic = 3,melee = 4,walking = 5
	};
	AnimationState m_AniState = Idle;
	float m_MaxActionAnimation;
	float m_ElapsedTime;
	bool m_IsReadyActionAnimation;
	DirectX::XMFLOAT3 m_InitialPosition;
	float m_Degrees;
	bool m_LateIni;
	bool m_ReadyWithAttackAnimation;
	
	bool m_StartSelfAnimation;
	//

	void SetTargetAndAttack(GameObject *GOenemy, int actionID);
	bool IsDead() { return m_Dead; };

	void SetSelected(bool isslected) { m_Selected = isslected; };

	void GiveAction(Action action) { m_Actions.push_back(action); };

private:

	void AnimationUpdate(const GameContext& gameContext);
	void DisplayTextUpdate(const GameContext& gameContext);
	void ConditionBuffUpdate(std::vector<ConditionBuff> &modifiers);

	void UpdateCondBuff(ConditionBuff &condBuff);
	void ApplyAttack(const Action& attack);

	void AddDefaultActionsNBuffs();

	void SelfAnimationUpdate(const GameContext& gameContext);
	void HitAnimationUpdate(const GameContext& gameContext);
	void DeadAnimation(const GameContext& gameContext);

	void AddParticle();
	ParticleEmitterComponent *m_pParticleEmitter;

	GameObject *m_GOenemy; 
	int m_ActionID;

	bool m_PlayHit;
	bool m_Dead;

	bool m_Selected;
	int m_MaxRandomExtraDamage;
	int m_LastDamage;
	SpriteFont* m_pFont;
	bool m_DisplayDamage;
	bool m_DisplayAttack;
	std::wstring m_LastAttackName;

	FMOD::Sound* m_pPunch;
	FMOD::Sound* m_pMagic;
};

