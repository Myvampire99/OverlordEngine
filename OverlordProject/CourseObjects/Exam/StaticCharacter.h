#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;

class StaticCharacter : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP
	};

	StaticCharacter(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~StaticCharacter() = default;

	StaticCharacter(const StaticCharacter& other) = delete;
	StaticCharacter(StaticCharacter&& other) noexcept = delete;
	StaticCharacter& operator=(const StaticCharacter& other) = delete;
	StaticCharacter& operator=(StaticCharacter&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

public:
	ControllerComponent* m_pController;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity,
		m_TerminalVelocity,
		m_Gravity,
		m_RunAccelerationTime,
		m_JumpAccelerationTime,
		m_RunAcceleration,
		m_JumpAcceleration,
		m_RunVelocity,
		m_JumpVelocity,
		m_Timer;
	DirectX::XMFLOAT3 m_Velocity;
};


