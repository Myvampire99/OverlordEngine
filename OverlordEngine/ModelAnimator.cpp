#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	UNREFERENCED_PARAMETER(clipNumber);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (m_pMeshFilter->m_AnimationClips.size() < clipNumber) {
		//If not,
			//	Call Reset
			//	Log a warning with an appropriate message
			//	return
		Reset();
		Logger::LogError(L"clipNumber > vector size");
		return;
	}
	else {
		//else
			//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
			//	Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (auto clip : m_pMeshFilter->m_AnimationClips) {
		if (clip.Name == clipName) {
			SetAnimation(clip);
			return;
		}
	}
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message

	Reset();
	Logger::LogError(L"No animation found with given string");


}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;


	//If m_ClipSet is true
	if (m_ClipSet) {
		m_Transforms = m_CurrentClip.Keys[0].BoneTransforms;
	}
	else {
		DirectX::XMFLOAT4X4 store;
		DirectX::XMStoreFloat4x4(&store, DirectX::XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, store);
	}
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//===== fmod ?

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}


		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount


		//for (auto key : m_CurrentClip.Keys) {
		//	if (key.Tick < m_TickCount) {
		//		keyA = key;
		//		break;
		//	}
		//}
		//for (auto key : m_CurrentClip.Keys) {
		//	if (key.Tick > m_TickCount) {
		//		keyB = key;
		//		break;
		//	}
		//}

		//for (auto key : m_CurrentClip.Keys) {

		//	if (key.Tick > m_TickCount && key.Tick < m_TickCount + 1) {
		//		keyB = key;
		//	}

		//	if (key.Tick < m_TickCount && key.Tick > m_TickCount - 1) {
		//		keyA = key;
		//	}
		//}



		for (size_t i{}; i < m_CurrentClip.Keys.size(); ++i) {
			if (m_CurrentClip.Keys[i].Tick > m_TickCount) {
				keyB = m_CurrentClip.Keys[i];

				if (i == 0)
					keyA = m_CurrentClip.Keys.back();
				else
					keyA = m_CurrentClip.Keys[i - 1];

				break;
			}
		}

		

		//auto keys = m_CurrentClip.Keys;
	///	auto JustAbove = std::lower_bound(keys.begin(), keys.end(), m_TickCount);

	/*	auto keys = m_CurrentClip.Keys;
		auto JustAbove = std::lower_bound(keys.begin(), keys.end(), m_TickCount);

		keyB = *JustAbove;
		if (JustAbove == keys.begin())
			keyA = keys.back();
		else
			keyA = *(--JustAbove);
*/

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float BlendFactor = 1.f - ((keyB.Tick - m_TickCount) / (keyB.Tick - keyA.Tick));
		//Clear the m_Transforms vector
		m_Transforms.clear();//===

		if (m_Transforms.size() < keyA.BoneTransforms.size())
			m_Transforms.resize(keyA.BoneTransforms.size());
		//FOR every boneTransform in a key (So for every bone)

		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++)
		{
			auto& transformA = keyA.BoneTransforms[i];
			DirectX::XMMATRIX matA = DirectX::XMLoadFloat4x4(&transformA);
			DirectX::XMVECTOR posA;
			DirectX::XMVECTOR rotA;
			DirectX::XMVECTOR sclA;
			DirectX::XMMatrixDecompose(&sclA, &rotA, &posA, matA);

			auto& transformB = keyB.BoneTransforms[i];
			DirectX::XMMATRIX matB = DirectX::XMLoadFloat4x4(&transformB);
			DirectX::XMVECTOR posB;
			DirectX::XMVECTOR rotB;
			DirectX::XMVECTOR sclB;
			DirectX::XMMatrixDecompose(&sclB, &rotB, &posB, matB);

			DirectX::XMVECTOR posC = DirectX::XMVectorLerp(posA, posB, BlendFactor);
			DirectX::XMVECTOR rotC = DirectX::XMQuaternionSlerp(rotA, rotB, BlendFactor);
			DirectX::XMVECTOR sclC = DirectX::XMVectorLerp(sclA, sclB, BlendFactor);
			DirectX::XMMATRIX matC = DirectX::XMMatrixAffineTransformation(sclC, DirectX::XMVECTOR{ 0 }, rotC, posC);
			DirectX::XMStoreFloat4x4(&m_Transforms[i], matC);
		}
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
	}
}
