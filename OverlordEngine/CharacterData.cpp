#include "stdafx.h"
#include "CharacterData.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "..\OverlordProject\CourseObjects\Exam\StaticCharacter.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "FloatingTextComponent.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "ParticleEmitterComponent.h"
#include "SoundManager.h"

CharacterData::CharacterData()
	:m_CharacterInfo{}
	, m_ReadyWithAttackAnimation{true}
	, m_WalkingSpeed{8}
	, m_StartAttacking{false}
	, m_StartActionAnimation{ false }
	, m_MaxActionAnimation{}
	, m_ElapsedTime{}
	, m_IsReadyActionAnimation{false}
	, m_InitialPosition{}
	, m_Degrees{0}
	, m_LateIni{true}
	, m_MaxHp{100}
	, m_MaxMp{100}
	, m_StartSelfAnimation{false}
	, m_PlayHit{false}
	, m_Dead{false}
	, m_Selected{false}
	, m_MaxRandomExtraDamage{5}
	, m_LastDamage{}
	, m_DisplayDamage{false}
	, m_DisplayAttack{false}
	, m_LastAttackName{}
, m_pFont{nullptr}
{
	
	AddDefaultActionsNBuffs();
}

void CharacterData::AddDefaultActionsNBuffs() {

	{	ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Default Mana Gain";
	defaultCondBuff.turnsRemaining = -1;
	defaultCondBuff.mpPerTurn = 10;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.f;
	defaultCondBuff.defense = 0.f;
	m_CurrentCondBuffs.push_back(defaultCondBuff); }


	{	ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Block";
	defaultCondBuff.turnsRemaining = 1;
	defaultCondBuff.mpPerTurn = 30;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.f;
	defaultCondBuff.defense = -0.24f;

	Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 0;
	defaultAction.name = L"Block";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	m_Actions.push_back(defaultAction);
	}

}

CharacterData::~CharacterData()
{
	SafeDelete(m_pFont);
}

void CharacterData::Draw(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);
}

void CharacterData::Initialize(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);
	
	
	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/Punch.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pPunch);
	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/warp.ogg", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pMagic);
	AddParticle();


}

void CharacterData::Update(const GameContext& gameContext) {
	UNREFERENCED_PARAMETER(gameContext);

	if (m_CharacterInfo.mp < 0)//Cannot Happen, But just in case
		m_CharacterInfo.mp = 0;
	
	if (m_CharacterInfo.hp < 0) {
		m_Dead = true;
	}

	if (m_CharacterInfo.defenseMod < 0.3f)
		m_CharacterInfo.defenseMod = 0.3f;
	
	DisplayTextUpdate(gameContext);

	if (!m_Dead) {
		AnimationUpdate(gameContext);
		SelfAnimationUpdate(gameContext);
		HitAnimationUpdate(gameContext);
	}
	else {
		DeadAnimation(gameContext);
	}



	m_Selected = false;

}

void CharacterData::HitAnimationUpdate(const GameContext& gameContext) {
	if (m_PlayHit) {

		float speed{6.f};
		
		if (m_AniState != Hit) {
			m_AniState = Hit;

			m_DisplayDamage = true;
			SoundManager::GetInstance()->GetSystem()->playSound(m_pPunch, 0, false, 0);

			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(Hit);
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->Play();
			m_MaxActionAnimation = m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetClipCount() / (m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetAnimationSpeed()*speed);
		}

		m_ElapsedTime += gameContext.pGameTime->GetElapsed();
		if (m_MaxActionAnimation < m_ElapsedTime) {
			m_ElapsedTime = 0;
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(Idle);
			m_PlayHit = false;
			m_AniState = Idle;
		
			m_DisplayDamage = false;
		}
	}
}

void CharacterData::NotifyStartTurn() {
	ConditionBuffUpdate(m_CurrentCondBuffs);

	//if dead
}

void CharacterData::ConditionBuffUpdate(std::vector<ConditionBuff> &modifiers) {

	std::vector<ConditionBuff> temp;
	for (auto &mod : modifiers) {
		UpdateCondBuff(mod);

		if (mod.turnsRemaining != 0)
			temp.push_back(mod);
	}

	modifiers = temp;

}

void CharacterData::UpdateCondBuff(ConditionBuff &condBuff) {

	m_CharacterInfo.defenseMod = 1.f;
	m_CharacterInfo.attackMod = 1.f;

	if (condBuff.turnsRemaining > 0 || condBuff.turnsRemaining == -1) {
		m_CharacterInfo.hp += condBuff.hpPerTurn;
		if (m_CharacterInfo.hp > m_MaxHp)
			m_CharacterInfo.hp = m_MaxHp;

		m_CharacterInfo.mp += condBuff.mpPerTurn;
		if (m_CharacterInfo.mp > m_MaxMp)
			m_CharacterInfo.mp = m_MaxMp;

		m_CharacterInfo.defenseMod += condBuff.defense;
		m_CharacterInfo.attackMod += condBuff.damageMod;

		if(condBuff.turnsRemaining != -1)
			condBuff.turnsRemaining--;
	}
}

bool CharacterData::IsReadyWithAction() {
	return m_IsReadyActionAnimation;
}

void CharacterData::StartAttackingAnimation(bool attack) {
	m_StartAttacking = attack;
	m_IsReadyActionAnimation = false;
}

void CharacterData::StartSelfAnimation(bool attack) {
	m_StartSelfAnimation = attack;
	m_IsReadyActionAnimation = false;
}

CharacterData::CharacterInfo& CharacterData::GetCharacterInfo() {
	return m_CharacterInfo;
}

void CharacterData::SetTargetAndAttack(GameObject *GOenemy, int actionID) {
	m_GOenemy = GOenemy;
	m_ActionID = actionID;
}
bool CharacterData::HasEnoughMP(const Action &action)
{
	bool temp =  action.mpCost > m_CharacterInfo.mp;
	return temp;
}

void CharacterData::DoAction(GameObject *GOenemy, int actionID) {

	ConditionBuffUpdate(m_Actions[actionID].Self_CondBuff);
	for (auto cond : m_Actions[actionID].Self_CondBuff)
		m_CurrentCondBuffs.push_back(cond);

	m_CharacterInfo.mp -= m_Actions[actionID].mpCost;

	Action modifiedAction{ m_Actions[actionID]};
	modifiedAction.damage = int(modifiedAction.damage*m_CharacterInfo.attackMod) + rand() % m_MaxRandomExtraDamage;

	if(GOenemy)
		GOenemy->GetComponent<CharacterData>()->DoActionOnMe(modifiedAction);

	m_LastAttackName = m_Actions[actionID].name;
}

void CharacterData::DoActionOnMe(Action &action) {

	for(auto cond : action.Enemy_CondBuff)
		m_CurrentCondBuffs.push_back(cond);

	ConditionBuffUpdate(action.Enemy_CondBuff);
	ApplyAttack(action);

	m_PlayHit = true;

}


void CharacterData::ApplyAttack(const Action& attack) {
	
	m_CharacterInfo.hp -= int(attack.damage*m_CharacterInfo.defenseMod);
	m_CharacterInfo.mp -= attack.mpDamage;

	m_LastDamage = int(attack.damage*m_CharacterInfo.defenseMod);

}

std::vector<CharacterData::ConditionBuff> CharacterData::GetConditionBuffs() {
	return m_CurrentCondBuffs;
}

std::vector<CharacterData::Action>& CharacterData::GetActions() {
	return m_Actions;
}

bool CharacterData::ReadyWithAttacking() {
	return m_ReadyWithAttackAnimation;
}

void CharacterData::DeadAnimation(const GameContext& gameContext) {

	float speed{3.5f};
	
	if (m_AniState != dying) {
		m_AniState = dying;
		m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(dying);
		m_MaxActionAnimation = m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetClipCount() / (m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetAnimationSpeed()*speed);
	}
	m_ElapsedTime += gameContext.pGameTime->GetElapsed();
	if (m_MaxActionAnimation < m_ElapsedTime) {

		m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->Pause();
	}

}

void CharacterData::SelfAnimationUpdate(const GameContext& gameContext) {
	float speed{ 2.0f };
	if (m_StartSelfAnimation) {
		if (m_AniState != magic) {
			m_DisplayAttack = true;

			auto offsetpos = m_pGameObject->GetTransform()->GetPosition();
			offsetpos.y += 1.f;
			offsetpos.x -= 0.5f;
			offsetpos.z -= 0.5f;
			m_pGameObject->GetChild<GameObject>()->GetTransform()->Translate(offsetpos);

			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(magic);
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->Play();
			m_MaxActionAnimation = m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetClipCount() / (m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetAnimationSpeed()*speed);
			m_AniState = magic;
			m_pParticleEmitter->SetPause(false);
			SoundManager::GetInstance()->GetSystem()->playSound(m_pMagic, 0, false, 0);
		}
		m_ElapsedTime += gameContext.pGameTime->GetElapsed();

		if (m_MaxActionAnimation / 2.f < m_ElapsedTime) {
			m_pParticleEmitter->SetMaxSize(5.0f);
			
		}
		if (m_MaxActionAnimation < m_ElapsedTime) {
			m_ElapsedTime = 0;
			m_IsReadyActionAnimation = true;
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(Idle);
			m_StartSelfAnimation = false;

			DoAction(m_GOenemy, m_ActionID);
			m_DisplayAttack = false;
			m_pParticleEmitter->SetMaxSize(1.0f);
			m_pParticleEmitter->SetPause(true);
			m_AniState = Idle;
		}
	}
}

void CharacterData::AnimationUpdate(const GameContext& gameContext) {
	if (m_StartAttacking && m_PlayHit == false) {// or if idle/walking
		m_DisplayAttack = true;

		DirectX::XMFLOAT3 Direction{};
		Direction.x = m_TargetPosition.x - m_pGameObject->GetTransform()->GetPosition().x;
		Direction.y = m_TargetPosition.y - m_pGameObject->GetTransform()->GetPosition().y;
		Direction.z = m_TargetPosition.z - m_pGameObject->GetTransform()->GetPosition().z;

		float length = sqrt(pow(Direction.x, 2) + pow(Direction.y, 2) + pow(Direction.z, 2));
		if (length < 1.f) {
			m_StartAttacking = false;
			m_StartActionAnimation = true;
		}

		DirectX::XMFLOAT3 normalized{};
		normalized.x = (Direction.x / length)*m_WalkingSpeed;
		normalized.y = 0.f;//(Direction.y / length)*m_WalkingSpeed;
		normalized.z = (Direction.z / length)*m_WalkingSpeed;

		dynamic_cast<StaticCharacter*>(m_pGameObject)->m_Velocity = normalized;


		if (m_AniState != walking) {
			m_AniState = walking;
			
			
			m_InitialPosition = m_pGameObject->GetTransform()->GetPosition();

			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(walking);
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->Play();

			float degrees{};

			if(Direction.z < 0)
				degrees = DirectX::XMConvertToDegrees(atan2(normalized.x, normalized.z) - 135.f );
			else
				degrees = DirectX::XMConvertToDegrees(atan2(normalized.x, normalized.z));

			
			dynamic_cast<StaticCharacter*>(m_pGameObject)->m_TotalYaw += degrees ;
			m_Degrees = degrees;
			m_DisplayAttack = false;
		}

	}
	else {
		dynamic_cast<StaticCharacter*>(m_pGameObject)->m_Velocity = { 0.f,0.f,0.f };
	}

	float speed{ 10.f };

	if (m_StartActionAnimation) {
		if (m_AniState != melee) {
			m_AniState = melee;

			DoAction(m_GOenemy,m_ActionID);

			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(melee);
			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->Play();

			m_MaxActionAnimation = m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetClipCount() / (m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->GetAnimationSpeed()*speed);
		}

		m_ElapsedTime += gameContext.pGameTime->GetElapsed();
		if (m_MaxActionAnimation < m_ElapsedTime) {
			m_ElapsedTime = 0;
			m_IsReadyActionAnimation = true;
			m_StartActionAnimation = false;

			m_pGameObject->GetTransform()->Translate(m_InitialPosition);

			m_pGameObject->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(Idle);

			dynamic_cast<StaticCharacter*>(m_pGameObject)->m_TotalYaw -= m_Degrees;
		}
	}

}

void CharacterData::DisplayTextUpdate(const GameContext& gameContext) {

	UNREFERENCED_PARAMETER(gameContext);

	//if (m_LateIni) {

	SafeDelete(m_pFont);
	m_pFont = new SpriteFont(ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Font64.fnt"));
	
	std::wstring healthPoints{};
	DirectX::XMFLOAT4 color{};

	if (!m_Selected)
		color = DirectX::XMFLOAT4(DirectX::Colors::White);
	else
		color = DirectX::XMFLOAT4(DirectX::Colors::Red);


	if (m_CharacterInfo.hp < 100)
		healthPoints = L"0" + std::to_wstring(m_CharacterInfo.hp);
	else
		healthPoints = std::to_wstring(m_CharacterInfo.hp);


	if (!m_DisplayDamage) {


			if (!m_Dead)
				m_pGameObject->GetComponent<FloatingTextComponent>()->DrawText(m_pFont, L"HP" + healthPoints, DirectX::XMFLOAT3{ -60.f,220.f,0.f }, color);//std::to_wstring(m_CharacterInfo.hp)
			else

				m_pGameObject->GetComponent<FloatingTextComponent>()->DrawText(m_pFont, L"DEAD", DirectX::XMFLOAT3{ -60.f,220.f,0.f }, DirectX::XMFLOAT4(DirectX::Colors::DarkRed));//std::to_wstring(m_CharacterInfo.hp)

	//	}
		}
	else {
			m_pGameObject->GetComponent<FloatingTextComponent>()->DrawText(m_pFont, std::to_wstring(m_LastDamage), DirectX::XMFLOAT3{ -60.f,270.f,0.f }, DirectX::XMFLOAT4(DirectX::Colors::Red));
		
	}
//m_LateIni = false;
//}
}

void CharacterData::AddParticle() {

	
	if (m_pGameObject->GetChild<GameObject>()->GetComponent<ParticleEmitterComponent>()) {
		m_pParticleEmitter = m_pGameObject->GetChild<GameObject>()->GetComponent<ParticleEmitterComponent>();
		m_pParticleEmitter->SetPause(true);
	}
	
//	else
	//error
}