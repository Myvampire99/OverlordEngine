#include "stdafx.h"
#include "ExamScene.h"
#include "GameObject.h"
#include "UIButtonComponent.h"
#include "TextRenderer.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "CharacterData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "../OverlordProject/Materials/ColorMaterial.h"
#include "..\..\CourseObjects\Exam\StaticCharacter.h"
#include "FloatingTextComponent.h"
#include "ParticleEmitterComponent.h"
#include "PhysxProxy.h"

#include "FixedCamera.h"
#include "../OverlordProject/Materials/Post/PostVignette.h"

#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../../Materials/SkyBoxMaterial.h"
#include "SoundManager.h"

#include "SceneManager.h"
#include "DebugRenderer.h"

ExamScene::ExamScene()
	:GameScene(L"ExamScene")
	, m_CurrentPlayer{0}
	, m_NeedUpdate{false}
	, m_BState{BS_ChooseEnemy}
	, m_GetPressedButtonNr{-1}
	, m_ForceUpdate{false}
	, debugCounter{0}
	, m_SkyBox{nullptr}
	, m_RotationSky{0}
	, m_pMenuSong{ nullptr }
	, m_IniUpdate{true}
	, m_ShowDebugCharacter{false}
{
}


ExamScene::~ExamScene()
{

	for (GameObject* friendly : m_FriendlyTeam)
	{
		for (GameObject* child : friendly->GetChildren<GameObject>())
		{
			friendly->RemoveChild(child);
			SafeDelete(child);
		}
	}

	for (GameObject* friendly : m_EnemyTeam)
	{
		for (GameObject* child : friendly->GetChildren<GameObject>())
		{
			friendly->RemoveChild(child);
			SafeDelete(child);
		}
	}



}

void ExamScene::Initialize() {

	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createStream("./Resources/Sound/Final_Fantasy_XIII_Battle_Theme.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pMenuSong);



	//SkyBox
	m_SkyBox = new GameObject();

	auto *skyMat = new SkyBoxMaterial();
	skyMat->SetSkyBoxMat(L"Resources/Textures/SkyBox2.dds");
	GetGameContext().pMaterialManager->AddMaterial(skyMat,5);
	m_SkyBox->AddComponent(new ModelComponent(L"Resources/Meshes/box.ovm"));
	m_SkyBox->GetComponent<ModelComponent>()->SetMaterial(5);
	m_SkyBox->GetTransform()->Rotate(0.f,0.f,90.f);
	AddChild(m_SkyBox);

	GetGameContext().pShadowMapper->SetLight({ -1.f,1.f,0.f }, { 0.740129888f, -0.597205281f, 0.309117377f });
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);
	DebugRenderer::ToggleDebugRenderer();

	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/AnimatedCharacter.png");
	skinnedDiffuseMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 0);

	skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/ely-vanguardsoldier-kerwinatienza_diffuse.png");
	skinnedDiffuseMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 3);

	skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/world_war_zombie_diffuse.png");
	skinnedDiffuseMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 4);



	auto pCamera = new FixedCamera();
	auto pCameraComponent = new CameraComponent();
	pCamera->AddComponent(pCameraComponent);
	AddChild(pCamera);
	SetActiveCamera(pCameraComponent);
	pCamera->GetTransform()->Translate(10.f, 5.6f,15.3f);
	pCamera->GetTransform()->Rotate(10.f,220.f, 0);


//GROUND PLANE
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	auto level = new GameObject();
	level->GetTransform()->Rotate(0.f,0.f,0.f);
	level->GetTransform()->Scale(2.3f, 2.3f,2.3f);
	level->GetTransform()->Translate(10.f, -1.9f, 0.f);
	auto levelComp = new ModelComponent(L"./Resources/Meshes/ground.ovm");
	auto cMat = new ColorMaterial(); 

	GetGameContext().pMaterialManager->AddMaterial(cMat, 61);
	levelComp->SetMaterial(61);
	level->AddComponent(levelComp);

	auto *diffuseMaterial = new DiffuseMaterial_Shadow();
	diffuseMaterial->SetDiffuseTexture(L"Resources/Textures/CityEnvi.tga");
	diffuseMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(diffuseMaterial, 2);
	level->GetComponent<ModelComponent>()->SetMaterial(2);
	AddChild(level);

	CreateCharacter(L"Medea", L"./Resources/Meshes/medea.ovm",0,10);
	CreateCharacter(L"N7",L"./Resources/Meshes/FriendCharacter.ovm",3);
	CreateCharacter(L"Zombie 1",L"./Resources/Meshes/EnemyCharacter.ovm",4,1,true);
	CreateCharacter(L"Zombie 2", L"./Resources/Meshes/EnemyCharacter.ovm",4,1,true);

	CreateNextUI();

	GetGameContext().pInput->AddInputAction(InputAction(0, Released, 'Q'));
	GetGameContext().pInput->AddInputAction(InputAction(1, Released, 'E'));

	GetGameContext().pInput->AddInputAction(InputAction(2, Released, 'R'));
	GetGameContext().pInput->AddInputAction(InputAction(3, Released, 'O'));




	auto NameHp = new GameObject();
	NameHp->AddComponent(new SpriteComponent(L"./Resources/Textures/NameHP.png"));
	AddChild(NameHp);



	AddPostProcessingEffect(new PostVignette());
	AddActions();
}

void ExamScene::Update() {

	FancySkyRotation();


	if (m_IniUpdate) {
		m_pSoundManager->GetSystem()->playSound(m_pMenuSong, 0, false, 0);
		m_IniUpdate = false;
	//	m_pMenuSong->setMusicChannelVolume(0, 0.01f);
	}

	if (GetGameContext().pInput->IsActionTriggered(2)) {
		//SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");
		SceneManager::GetInstance()->NextScene();
		m_pSoundManager->GetSystem()->playSound(m_pMenuSong, 0, true, 0);
	
		m_IniUpdate = true;
	}
		

	if (!m_ForceUpdate) {
		if (m_BState == BS_NextCharacter)
			m_NeedUpdate = m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->ReadyWithAttacking();
		else
			m_NeedUpdate = CheckIfPressed();
	}
	else {
		m_NeedUpdate = true;
		//m_ForceUpdate = false;
	}

	if (m_BState == BS_NextCharacter) {
		if (m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->IsReadyWithAction()) {
			m_NeedUpdate = true;
		}
		else {
			m_NeedUpdate = false;
		}
	}

	if(m_BState == BS_EnemyBattle && m_CurrentPlayer != 0)
	{
		if (m_EnemyTeam[m_CurrentPlayer-1]->GetComponent<CharacterData>()->IsReadyWithAction()) {
			m_NeedUpdate = true;
			
			if (m_CurrentPlayer >= (int)m_EnemyTeam.size())
			{
				m_BState = BS_ChooseEnemy;
				m_CurrentPlayer = 0;
			}
			
		}
		else {
			m_NeedUpdate = false;
		}
	}

	if(m_CurrentPlayer == 0 && m_BState == BS_EnemyBattle)
	{
		m_NeedUpdate = true;
	}

	int nr{};
	for(auto friends : m_FriendlyTeam)
	{
		if (friends->GetComponent<CharacterData>()->IsDead())
			nr++;
	}
	if(nr == (int)m_FriendlyTeam.size() && m_BState != BS_ENDGAME)
	{
		m_ForceUpdate = true;
		m_BState = BS_Defeated;
	}

	nr = 0;
	for (auto friends : m_EnemyTeam)
	{
		if (friends->GetComponent<CharacterData>()->IsDead())
			nr++;
	}
	if (nr == (int)m_EnemyTeam.size() && m_BState != BS_ENDGAME)
	{
		m_ForceUpdate = true;
		m_BState = BS_Won;
	}


	if(m_BState == BS_ENDGAME)
	{
		if (m_GetPressedButtonNr == 0)
			m_ForceUpdate = true;
	}
	


	if (m_NeedUpdate || m_ForceUpdate) {
		m_ForceUpdate = false;
		if ((m_BackNumber == m_GetPressedButtonNr) && m_BState == BS_Attacking) {
			m_BState = BS_ChooseEnemy;
		}

			DeletePrevUI();
			CreateNextUI();
			m_NeedUpdate = false;


			
	}



	if(m_BState == BS_ChooseAttack)
		DrawCharacterInfo();

	
		CurrentPlayerInfoDraw();
	

	if(m_BState == BS_Attacking)
		DrawActionsInfo();

	if (GetGameContext().pInput->IsActionTriggered(3))
	{
		m_ShowDebugCharacter =  !m_ShowDebugCharacter;
	}
	
	if (GetGameContext().pInput->IsActionTriggered(0))
	{
		debugCounter--;
		if (debugCounter < 0)
			debugCounter = m_FriendlyTeam.size() + m_EnemyTeam.size() - 1;
	}

	if (GetGameContext().pInput->IsActionTriggered(1))
	{
		debugCounter++;
		if (debugCounter > int(m_FriendlyTeam.size()+ m_EnemyTeam.size() - 1)) {
			debugCounter = 0;
		}
	}

	if (m_ShowDebugCharacter) {
		if (debugCounter >= (int)m_FriendlyTeam.size())
			DebugDrawInfo(m_EnemyTeam[debugCounter - m_FriendlyTeam.size()]);
		else
			DebugDrawInfo(m_FriendlyTeam[debugCounter]);
	}
	}

void ExamScene::CurrentPlayerInfoDraw() {


	if (!(m_CurrentPlayer >= (int)m_FriendlyTeam.size())) {
		auto m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");
		auto comp = m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>();

		TextRenderer::GetInstance()->DrawText(m_pFont, comp->GetCharacterInfo().name, DirectX::XMFLOAT2(m_LeftMarge, m_LeftMarge), DirectX::XMFLOAT4(DirectX::Colors::White));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Health Points : " + std::to_wstring(comp->GetCharacterInfo().hp), DirectX::XMFLOAT2(m_LeftMarge, m_LeftMarge + m_WindowMargin * 2.f), DirectX::XMFLOAT4(DirectX::Colors::IndianRed));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Mana Points   : " + std::to_wstring(comp->GetCharacterInfo().mp), DirectX::XMFLOAT2(m_LeftMarge, m_LeftMarge + m_WindowMargin * 4.f), DirectX::XMFLOAT4(DirectX::Colors::BlueViolet));
	}

}

void ExamScene::DrawActionsInfo() {

	for (int i{}; i < (int)m_UIButtons.size(); ++i) {
		if (m_BackNumber != m_UIButtons[i]->GetComponent<UIButtonComponent>()->GetStoredNumber()) {
			auto m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_32.fnt");
			auto m_pFont2 = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_20.fnt");
			CharacterData* comp = nullptr;
			comp = m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>();

			auto pos = m_UIButtons[i]->GetTransform()->GetPosition();

			TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(comp->GetActions()[m_UIButtons[i]->GetComponent<UIButtonComponent>()->GetStoredNumber()].mpCost), DirectX::XMFLOAT2(276, pos.y + 5), DirectX::XMFLOAT4(DirectX::Colors::White));
			TextRenderer::GetInstance()->DrawText(m_pFont2, L"Mana", DirectX::XMFLOAT2(253, pos.y + 29), DirectX::XMFLOAT4(DirectX::Colors::White));
		}
	}

}

void ExamScene::DrawCharacterInfo() {

	for (int i{};i<(int)m_UIButtons.size();++i){
		if (m_UIButtons[i]->GetComponent<UIButtonComponent>()->IsHovering()) {

			auto m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_20.fnt");
			CharacterData* comp = nullptr;


			if (i != (int)m_UIButtons.size()-1) {
				comp = m_EnemyTeam[m_UIButtons[i]->GetComponent<UIButtonComponent>()->GetStoredNumber()]->GetComponent<CharacterData>();
			}
			else {
				comp = m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>();
			}

			comp->SetSelected(true);

			auto pos = m_UIButtons[i]->GetTransform()->GetPosition();

			TextRenderer::GetInstance()->DrawText(m_pFont, L"HP" + std::to_wstring(comp->GetCharacterInfo().hp), DirectX::XMFLOAT2(276, pos.y + 5 ), DirectX::XMFLOAT4(DirectX::Colors::White));
			TextRenderer::GetInstance()->DrawText(m_pFont, L"MP" + std::to_wstring(comp->GetCharacterInfo().mp), DirectX::XMFLOAT2(255, pos.y + 25), DirectX::XMFLOAT4(DirectX::Colors::White));

			float blockSize{ 20.f };
			int counter{};
			for (auto mod : comp->GetConditionBuffs()) {
				if (mod.turnsRemaining != -1){
					TextRenderer::GetInstance()->DrawText(m_pFont, L"Cond: " + mod.name + std::to_wstring(mod.turnsRemaining), DirectX::XMFLOAT2(10, m_UIButtons.back()->GetTransform()->GetPosition().y + 60 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::White));
						counter++;
				}
			}

		}
	}

}

void ExamScene::DebugDrawInfo(GameObject* character) {

	auto m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Consolas_20.fnt");
	auto comp = character->GetComponent<CharacterData>();

	TextRenderer::GetInstance()->DrawText(m_pFont, comp->GetCharacterInfo().name, DirectX::XMFLOAT2(1000, 10), DirectX::XMFLOAT4(DirectX::Colors::DarkGray));

	TextRenderer::GetInstance()->DrawText(m_pFont, L"HP : "+std::to_wstring(comp->GetCharacterInfo().hp) , DirectX::XMFLOAT2(1000, 40), DirectX::XMFLOAT4(DirectX::Colors::Black));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"MP : " + std::to_wstring(comp->GetCharacterInfo().mp), DirectX::XMFLOAT2(1000, 60), DirectX::XMFLOAT4(DirectX::Colors::Black));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Def: " + std::to_wstring(comp->GetCharacterInfo().defenseMod*100.f), DirectX::XMFLOAT2(1000, 80), DirectX::XMFLOAT4(DirectX::Colors::Black));
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Att: " + std::to_wstring(comp->GetCharacterInfo().attackMod*100.f), DirectX::XMFLOAT2(1000, 100), DirectX::XMFLOAT4(DirectX::Colors::Black));

	float blockSize{130.f};
	int counter{};
	for (auto mod : comp->GetConditionBuffs()) {
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Debuf: " + mod.name, DirectX::XMFLOAT2(1000, 130 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::DarkGray));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"hp pt: " + std::to_wstring(mod.hpPerTurn), DirectX::XMFLOAT2(1000, 150 + counter* blockSize), DirectX::XMFLOAT4(DirectX::Colors::Black));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"mp pt: " + std::to_wstring(mod.mpPerTurn), DirectX::XMFLOAT2(1000, 170 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::Black));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"def  : " + std::to_wstring(mod.defense), DirectX::XMFLOAT2(1000, 190 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::Black));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"att  : " + std::to_wstring(mod.damageMod), DirectX::XMFLOAT2(1000, 210 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::Black));
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Turns: " + std::to_wstring(mod.turnsRemaining), DirectX::XMFLOAT2(1000, 230 + counter * blockSize), DirectX::XMFLOAT4(DirectX::Colors::DarkGray));

		counter++;
	}

}

void ExamScene::Draw() {
	
}

void ExamScene::CreateButton(std::wstring text,DirectX::XMFLOAT3 position,int numberIfPressed,bool disabled) {

	auto Button = new GameObject;
	Button->GetTransform()->Translate(position);
	auto comp = new SpriteComponent(L"Resources/Textures/UIButton.png", DirectX::XMFLOAT2(0.0f, 0.0f));

	Button->AddComponent(comp);
	Button->AddComponent(new UIButtonComponent(text, comp));
	Button->GetTransform()->Scale(0.28f, 0.28f, 1.f);
	Button->GetComponent<UIButtonComponent>()->SetPressed(numberIfPressed);
	Button->GetComponent<UIButtonComponent>()->SetDisabled(disabled);

	m_UIButtons.push_back(Button);
	AddChild(Button);
}


void ExamScene::CreateNextUI() {

	if(m_BState == BS_ChooseEnemy)
	{
		if (m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->IsDead())
			m_BState = BS_NextCharacter;
	}

	
	switch (m_BState) {
	case BS_ChooseEnemy:

		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->NotifyStartTurn();
		CreateUIChooseEnemy();


		m_BState = BS_ChooseAttack;
		break;
	case BS_ChooseAttack:
		m_Target = m_GetPressedButtonNr;
		CreateUIChooseAttack();

		
	
		m_BState = BS_Attacking;
		break;
	case BS_Attacking:
		m_ActionID = m_GetPressedButtonNr;
		CompleteAction();
		
		
		
		m_BState = BS_NextCharacter;
		break;
	case BS_NextCharacter:

		m_CurrentPlayer++;
		if (m_CurrentPlayer == (int)m_FriendlyTeam.size())
		{
			
			m_BState = BS_EnemyBattle;
			m_CurrentPlayer = 0;

			
		}
		else
			m_BState = BS_ChooseEnemy;

		m_ForceUpdate = true;

		break;
	case BS_EnemyBattle:
		std::cout << std::to_string(m_CurrentPlayer)<<std::endl;
		
		if (!m_EnemyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->IsDead())
			ChooseAttackAi();
		else
		{
			m_CurrentPlayer++;
			m_ForceUpdate = true;
			if (m_CurrentPlayer >= (int)m_EnemyTeam.size())
			{
				m_BState = BS_ChooseEnemy;
				m_CurrentPlayer = 0;
			}
		}
		break;
	case BS_Defeated:
		m_ENDGAME = true;
		m_BState = BS_ENDGAME;
		CreateButton(L"DEFEATED", { 200.f,200.f,1.f }, 0);
		break;
	case BS_Won:
		m_ENDGAME = true;
		m_BState = BS_ENDGAME;
		CreateButton(L"VICTORY", { 200.f,200.f,1.f }, 0);
		break;
	case BS_ENDGAME:
	
		SceneManager::GetInstance()->SetActiveGameScene(L"MenuScene");
		
		break;
	default:
		break;
	}

}

void ExamScene::ChooseAttackAi()
{
	int i = m_CurrentPlayer;
	std::pair<CharacterData::Action, GameObject*> output{};
	output = AIActions(m_EnemyTeam[i], m_FriendlyTeam, m_EnemyTeamBuffTurns[i]);

	if (output.second != nullptr)
	{
		m_EnemyTeam[i]->GetComponent<CharacterData>()->m_TargetPosition = output.second->GetTransform()->GetPosition();
		m_EnemyTeam[i]->GetComponent<CharacterData>()->StartAttackingAnimation(true);
	}
	else
	{
		m_EnemyTeam[i]->GetComponent<CharacterData>()->m_TargetPosition = {};
		m_EnemyTeam[i]->GetComponent<CharacterData>()->StartSelfAnimation(true);
	}

	for (int j{}; j < (int)m_EnemyTeam[i]->GetComponent<CharacterData>()->GetActions().size(); ++j)
	{
		if (m_EnemyTeam[i]->GetComponent<CharacterData>()->GetActions()[j].name == output.first.name)
		{
			m_EnemyTeam[i]->GetComponent<CharacterData>()->SetTargetAndAttack(output.second, j);
		}
	}

	//}

	std::wcout <<  output.first.name << std::endl;

	m_CurrentPlayer++;

		
}

std::pair<CharacterData::Action, GameObject*>  ExamScene::AIActions(GameObject* character, std::vector<GameObject*> enemies, int &TurnsToBuff)
{
	std::pair<CharacterData::Action, GameObject*> output{};
	auto comp = character->GetComponent<CharacterData>();


	//Choose lowest HP Target
	int lowestHp{ enemies[0]->GetComponent<CharacterData>()->GetCharacterInfo().hp};
	for(auto enemy: enemies){
		auto info = enemy->GetComponent<CharacterData>()->GetCharacterInfo();
		if(info.hp <= lowestHp && info.hp > 0){
			lowestHp = info.hp;
			output.second = enemy;
		}
	}
	//
	//If Low Hp Enemy
	if (output.second->GetComponent<CharacterData>()->GetCharacterInfo().hp < 15) {
		for (auto attack : comp->GetActions()) {
			if (attack.name == L"Bite" && !comp->HasEnoughMP(attack)) {
				output.first = attack;
				return output;
			}
		}

	}
	//

	//If Low Hp Self
	if (comp->GetCharacterInfo().hp < 25) {
		for (auto attack : comp->GetActions()) {
			if (attack.name == L"Heal" && !comp->HasEnoughMP(attack)) {
				output.first = attack;
				output.second = nullptr;
				return output;
			}
		}

	}
	//
	
	//if Low Mp
	if (comp->GetCharacterInfo().mp < 20) {
		for (auto attack : comp->GetActions()) {
			if (attack.name == L"Block" && !comp->HasEnoughMP(attack)) {
				output.first = attack;
				output.second = nullptr;
				return output;
			}
		}

	}
	//
	
	//Apply Buff to himself (Random So it will not activate exactly on the counter)
	TurnsToBuff--;
	if(TurnsToBuff < 1){
		
		std::vector<CharacterData::Action> staticActions;
		for(auto action : comp->GetActions())
		{
			if(action.Static == true)
			{
				staticActions.push_back(action);
			}
		}

		int randomID = rand() % (int)staticActions.size();
		output.first = staticActions[randomID];
		output.second = nullptr;

		if (!comp->HasEnoughMP(output.first)) {
			TurnsToBuff = 4;
			return output;
		}
	}
	//

	//Random Attack
	std::vector<CharacterData::Action> randomAction;
	for (auto attack : comp->GetActions()) {
		if (attack.Static == false && !(comp->HasEnoughMP(attack))) {
			randomAction.push_back(attack);
		}
	}
	
	int randomID = rand() % (int)randomAction.size();
	output.first = randomAction[randomID];
	return output;
	//

}

void ExamScene::CreateUIChooseEnemy() {

	for (int i{}; i < (int)m_EnemyTeam.size();++i) {
		auto info = m_EnemyTeam[i]->GetComponent<CharacterData>()->GetCharacterInfo();
		if (!m_EnemyTeam[i]->GetComponent<CharacterData>()->IsDead()) {
			CreateButton(info.name, { m_WindowMargin ,m_WindowMargin + m_HeightMargin * i + m_NameMargin,0.f }, i);//+ m_FriendlyTeam.size()
		}
		else {
			CreateButton(info.name, { m_WindowMargin ,m_WindowMargin + m_HeightMargin * i + m_NameMargin,0.f }, i,true);//+ m_FriendlyTeam.size()
		}
	}

	CreateButton(L"Self", { m_WindowMargin ,m_WindowMargin + m_HeightMargin * m_EnemyTeam.size() + m_NameMargin,0.f }, -2);

	
}

bool ExamScene::CheckIfPressed() {

	for (auto button : m_UIButtons) {
		int nr = button->GetComponent<UIButtonComponent>()->GetPressed();
		if (nr == -2) {
			m_GetPressedButtonNr = -1;
			return true;
		}
		if (nr != -1) {
			m_GetPressedButtonNr = nr;
			return true;
		}
	}
	return false;
}

void ExamScene::FancySkyRotation() {

	float speed{ 1.f };
	m_RotationSky += speed * GetGameContext().pGameTime->GetElapsed();

	if (m_RotationSky > 360.f)
		m_RotationSky = 0;

	m_SkyBox->GetTransform()->Rotate(0.f, m_RotationSky, 90.f);

}

void ExamScene::CreateCharacter(std::wstring name, std::wstring path, int matId, int scale, bool IsEnemy) {

	auto LocalCharacter = new StaticCharacter(0.3f,1.f);
	LocalCharacter->AddComponent(new FloatingTextComponent());

	auto component = new CharacterData();
	LocalCharacter->AddComponent(component);
	component->GetCharacterInfo().name = name;

	//Particle
	auto m_GameObjectChild = new GameObject();
	auto m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Particle3.png", 100);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0.f, 1.0f, 0.0f));
	m_pParticleEmitter->SetMinSize(0.1f);
	m_pParticleEmitter->SetMaxSize(0.3f);
	m_pParticleEmitter->SetMinEnergy(0.1f);
	m_pParticleEmitter->SetMaxEnergy(0.3f);
	m_pParticleEmitter->SetMinSizeGrow(1.5f);
	m_pParticleEmitter->SetMaxSizeGrow(2.5f);
	m_pParticleEmitter->SetMinEmitterRange(0.2f);
	m_pParticleEmitter->SetMaxEmitterRange(0.5f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));
	m_GameObjectChild->AddComponent(m_pParticleEmitter);
	LocalCharacter->AddChild(m_GameObjectChild);
	//

	//Character Animation
	auto m_pModel = new ModelComponent(path);
	LocalCharacter->AddComponent(m_pModel);
	LocalCharacter->GetTransform()->Scale(0.01f*scale, 0.01f*scale, 0.01f*scale);
	AddChild(LocalCharacter);

	m_pModel->SetMaterial(matId);
	m_pModel->GetAnimator()->SetAnimation(0);
	m_pModel->GetAnimator()->Play();

	//


	if (IsEnemy) {
		m_EnemyTeamBuffTurns.push_back(0);
		
		LocalCharacter->GetTransform()->Translate(m_CharacterSpace*m_EnemyTeam.size()+3.f, 1.f, 1.f);
		LocalCharacter->m_TotalYaw = 180.f;
		m_EnemyTeam.push_back(LocalCharacter);
	}	
	else {
		LocalCharacter->GetTransform()->Translate(m_CharacterSpace*m_FriendlyTeam.size(), 0.f, m_CharacterTeamSpace);

		m_FriendlyTeam.push_back(LocalCharacter);
	}
		
}

void ExamScene::DeletePrevUI() {
	for(auto button : m_UIButtons)
	RemoveChild(button);

	m_UIButtons.clear();
}

void ExamScene::CreateUIChooseAttack() {

	int i{};//visual
	int counter{};//action iD
	for (auto action : m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->GetActions()) {
		if (m_Target != -1) {
			if (!action.Static) {
				CreateButton(action.name, { m_WindowMargin ,m_WindowMargin + m_HeightMargin * i + m_NameMargin,0.f }, counter, m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->HasEnoughMP(action));
				++i;
			}
		}
		else {
			if (action.Static) {
				CreateButton(action.name, { m_WindowMargin ,m_WindowMargin + m_HeightMargin * i + m_NameMargin,0.f }, counter, m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->HasEnoughMP(action));
				++i;
			}
		}
		counter++;
	}

	CreateButton(L"BACK", { m_WindowMargin ,m_WindowMargin + 20.f+ m_HeightMargin * i + m_NameMargin,0.f }, -3);


}

void ExamScene::CompleteAction() {

	if (m_Target != -1) {
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->m_TargetPosition = m_EnemyTeam[m_Target]->GetTransform()->GetPosition();
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->StartAttackingAnimation(true);
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->SetTargetAndAttack(m_EnemyTeam[m_Target], m_ActionID);
	}

	if (m_Target == -1) {
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->m_TargetPosition = {};
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->StartSelfAnimation(true);
		m_FriendlyTeam[m_CurrentPlayer]->GetComponent<CharacterData>()->SetTargetAndAttack(nullptr, m_ActionID);
	}

}


void ExamScene::AddActions() {

	//**************
	//Character  1

	auto character = m_FriendlyTeam[0]->GetComponent<CharacterData>();
	
	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 10;
	defaultAction.mpCost = 10;
	defaultAction.name = L"Light Attack";
	defaultAction.Static = false;
	character->GiveAction(defaultAction);
	}

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 25;
	defaultAction.mpCost = 25;
	defaultAction.name = L"Heavy Attack";
	defaultAction.Static = false;
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Poison";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = -5;
	defaultCondBuff.damageMod = 0.f;
	defaultCondBuff.defense = 0.f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 20;
	defaultAction.name = L"Poison Slash";
	defaultAction.Static = false;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }
	
	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Life Bleed";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = -5;
	defaultCondBuff.damageMod = 0.f;
	defaultCondBuff.defense = 0.f;

	CharacterData::ConditionBuff defaultCondBuff2{};
	defaultCondBuff2.name = L"Life Regen";
	defaultCondBuff2.turnsRemaining = 3;
	defaultCondBuff2.mpPerTurn = 0;
	defaultCondBuff2.hpPerTurn = 5;
	defaultCondBuff2.damageMod = 0.f;
	defaultCondBuff2.defense = 0.f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 5;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 50;
	defaultAction.name = L"Life Drain";
	defaultAction.Static = false;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	defaultAction.Self_CondBuff.push_back(defaultCondBuff2);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Def Up";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.f;
	defaultCondBuff.defense = -0.5f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 30;
	defaultAction.name = L"Def up";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Attack Up";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.5f;
	defaultCondBuff.defense = 0.f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 30;
	defaultAction.name = L"Attack up";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Weaken";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.5f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 15;
	defaultAction.name = L"Weaken";
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Self Heal";
	defaultCondBuff.turnsRemaining = 1;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 50;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 40;
	defaultAction.name = L"Heal";
	defaultAction.Static = true;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	//**************
	//Character  2

	character = m_FriendlyTeam[1]->GetComponent<CharacterData>();

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 10;
	defaultAction.mpCost = 10;
	defaultAction.name = L"Quick Attack";
	defaultAction.Static = false;
	character->GiveAction(defaultAction);
	}

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 5;
	defaultAction.mpDamage = 50;
	defaultAction.mpCost = 10;
	defaultAction.name = L"Disturb Mana";
	defaultAction.Static = false;
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Mana Poison";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = -5;
	defaultCondBuff.hpPerTurn = -5;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;
		
		CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 40;
	defaultAction.name = L"Mana Poison";
	defaultAction.Static = false;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Self Heal";
	defaultCondBuff.turnsRemaining = 1;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 50;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 40;
	defaultAction.name = L"Heal";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Regeneration";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 20;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 20;
	defaultAction.name = L"Regen";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Bleed Weaken Super Slash";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = -5;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.5f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 40;
	defaultAction.mpDamage = 10;
	defaultAction.mpCost = 100;
	defaultAction.name = L"Super Slash";
	defaultAction.Static = false;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }


	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Mana Regen";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 20;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 20;
	defaultAction.name = L"MP Regen";
	defaultAction.Static = true;
	defaultAction.Enemy_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }


	//**************
	//Enemy 1

	character = m_EnemyTeam[0]->GetComponent<CharacterData>();

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 10;
	defaultAction.mpCost = 10;
	defaultAction.name = L"Bite";
	defaultAction.Static = false;
	character->GiveAction(defaultAction);
	}

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 20;
	defaultAction.mpCost = 20;
	defaultAction.name = L"Slam";
	defaultAction.Static = false;
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Attack Up";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.5f;
	defaultCondBuff.defense = 0.f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 30;
	defaultAction.name = L"Frenzy";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Self Heal";
	defaultCondBuff.turnsRemaining = 1;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 50;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 40;
	defaultAction.name = L"Heal";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	//**************
//Enemy 2

	character = m_EnemyTeam[1]->GetComponent<CharacterData>();

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 10;
	defaultAction.mpCost = 10;
	defaultAction.name = L"Bite";
	defaultAction.Static = false;
	character->GiveAction(defaultAction);
	}

	{	CharacterData::Action defaultAction{};
	defaultAction.damage = 20;
	defaultAction.mpCost = 20;
	defaultAction.name = L"Slam";
	defaultAction.Static = false;
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Attack Up";
	defaultCondBuff.turnsRemaining = 3;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 0;
	defaultCondBuff.damageMod = 0.5f;
	defaultCondBuff.defense = 0.f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 30;
	defaultAction.name = L"Frenzy";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }

	{	CharacterData::ConditionBuff defaultCondBuff{};
	defaultCondBuff.name = L"Self Heal";
	defaultCondBuff.turnsRemaining = 1;
	defaultCondBuff.mpPerTurn = 0;
	defaultCondBuff.hpPerTurn = 50;
	defaultCondBuff.damageMod = 0.0f;
	defaultCondBuff.defense = 0.0f;

	CharacterData::Action defaultAction{};
	defaultAction.damage = 0;
	defaultAction.mpDamage = 0;
	defaultAction.mpCost = 40;
	defaultAction.name = L"Heal";
	defaultAction.Static = true;
	defaultAction.Self_CondBuff.push_back(defaultCondBuff);
	character->GiveAction(defaultAction); }
	
}