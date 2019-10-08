
#include "stdafx.h"
#include "StaticCharacter.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "FixedCamera.h"
#include "DirectXMath.h"

StaticCharacter::StaticCharacter(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	m_MaxRunVelocity(1.0f),
	m_TerminalVelocity(20),
	m_Gravity(9.81f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),
	m_Timer(0.f)
{}

void StaticCharacter::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = physX->createMaterial(0.5, 0.5, 0.5);
	m_pController = new ControllerComponent(pDefaultMaterial, m_Radius, m_Height);
	AddComponent(m_pController);

}

void StaticCharacter::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void StaticCharacter::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto look = physx::PxVec2(0, 0); 
	GetTransform()->Rotate(0.0f, m_TotalYaw, 0.0f);

	//GRAVITY
	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN)) {

		if (m_Timer > gameContext.pGameTime->GetTotal())
			m_Velocity.y += m_JumpAcceleration * gameContext.pGameTime->GetElapsed();
		else
			m_Velocity.y -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();

		if (m_JumpVelocity > m_TerminalVelocity)
			m_JumpVelocity = m_TerminalVelocity;

	}
	else if (gameContext.pInput->IsActionTriggered(JUMP)) {
		m_Velocity.y = m_TerminalVelocity;
		m_Timer = gameContext.pGameTime->GetTotal() + m_JumpAccelerationTime;
	}
	else {
		m_Velocity.y = 0;
	}

	m_Velocity.x *= gameContext.pGameTime->GetElapsed();
	m_Velocity.z *= gameContext.pGameTime->GetElapsed();
	m_pController->Move(m_Velocity);

}