#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "RigidBodyComponent.h"

CameraComponent::CameraComponent():
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(true),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());
}

void CameraComponent::Initialize(const GameContext&) {}

void CameraComponent::Update(const GameContext&)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if(m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio ,m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size>0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size>0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const DirectX::XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext&) {}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if(gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if(gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(ignoreGroups);
		
	//TODO implement

	POINT mousePos = gameContext.pInput->GetMousePosition();
	float halfWidth = OverlordGame::GetGameSettings().Window.Width / 2.f;
	float halfHeight = OverlordGame::GetGameSettings().Window.Height / 2.f;

	float Xndc = (mousePos.x - halfWidth) / halfWidth;
	float Yndc = (mousePos.y - halfHeight) / halfHeight;

	
	//NearPoint
		DirectX::XMVECTOR ndcVector = DirectX::XMVectorSet(Xndc, Yndc, 0, 0);
		auto ndcVectorMatrix = DirectX::XMMatrixTranslationFromVector(ndcVector);

		DirectX::XMFLOAT4X4 invView = gameContext.pCamera->GetViewProjectionInverse();
		DirectX::XMMATRIX invViewMatrix = DirectX::XMLoadFloat4x4(&invView);

		DirectX::XMMATRIX nearpoint = DirectX::XMMatrixMultiply(ndcVectorMatrix, invViewMatrix);

		DirectX::XMStoreFloat4x4(&invView, nearpoint);
		physx::PxVec3 raystart;
		raystart.x = invView._11;
		raystart.y = invView._12;
		raystart.z = invView._13;
		///auto nearpoint = DirectX::XMFLOAT4(Xndc, Yndc, 0, 0) * XMStoreFloat4x4(gameContext.pCamera->GetViewProjectionInverse());
	//

	//FarPoint
	ndcVector = DirectX::XMVectorSet(Xndc, Yndc, 1, 0);
	ndcVectorMatrix = DirectX::XMMatrixTranslationFromVector(ndcVector);

	invView = gameContext.pCamera->GetViewProjectionInverse();
	invViewMatrix = DirectX::XMLoadFloat4x4(&invView);

	DirectX::XMMATRIX farpoint = DirectX::XMMatrixMultiply(ndcVectorMatrix, invViewMatrix);

	DirectX::XMStoreFloat4x4(&invView, farpoint);
	physx::PxVec3 rayDir;
	rayDir.x = invView._11;//raystart.x = invView._11;
	rayDir.y = invView._12;
	rayDir.z = invView._13;

	rayDir.normalize();
	//rayDir *= rayDir.magnitude();


	//rayDir.normalize();// normalize();//=========
	///auto nearpoint = DirectX::XMFLOAT4(Xndc, Yndc, 0, 0) * XMStoreFloat4x4(gameContext.pCamera->GetViewProjectionInverse());
	//


	
	//raycast
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~ignoreGroups;
	
	physx::PxRaycastBuffer hit;
	auto activeScene = SceneManager::GetInstance()->GetActiveScene();
	
	if (activeScene->GetPhysxProxy()->Raycast(raystart, rayDir, PX_MAX_F32, hit, physx::PxHitFlag::eDEFAULT, filterData)) {
		if (hit.getAnyHit(0).actor->userData) {
			return static_cast<RigidBodyComponent*>(hit.getAnyHit(0).actor->userData)->GetGameObject();
		}
	}
	
//

	return nullptr;
}