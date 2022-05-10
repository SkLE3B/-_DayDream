#include "Player.h"
#include "Input/Input.h"
#include "2d/DebugText.h"
#include "Collision/SphereCollider.h"
#include "3d/ParticleManager.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Input/Gamepad.h"
#include "Collision/MeshCollider.h"
#include "Camera/DebugCamera.h"
#include "Math/Easing.h"
#include "time.h"
#include <windows.h>
#pragma comment(lib, "winmm.lib")

using namespace DirectX;

Camera* Player::camera = nullptr;

std::shared_ptr<Player> Player::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	std::shared_ptr<Player> instance = std::make_shared<Player>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(camera)) {
		instance.reset();
		assert(0);
	}

	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

bool Player::Initialize(Camera* camera)
{
	if (!Object3d::Initialize())
	{
		return false;
	}
	Player::camera = camera;

	prePos = position;
	direction = { 0,0,1 };
	stamina = 2.0f;
	cameraRotCount = 0;
	rotCamera = false;
	totalTime = 0;
	radY = 0;
	maxTime = 0.3f;//全体時間[s]
	timeRate;
	elapsedTime;
	// コライダーの追加
	radius = 2.0f;
	SetCollider(new SphereCollider(XMVECTOR({0,radius,0,0 }), radius),COLLISION_ATTR_ALLIES);
	lapseTime = 0.0f;
    distance = 10.0f;
	collisionFlag = false; 
	timerFlag = false;
	actionFlag = false;
	dashFlag = false;
	ActionState = STARTANIME;
	correctionPos = { 0,0,0 };
	SP = new ExNum(10.0f,0.0f,10.0f);
	HP = new ExNum(800.0f, 0.0f, 800.0f);

	cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
	cameraDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();
	moveDirection = 0;
	cameraTarget = { 0,1,0 };
	rotation = { 0,0,0 };
	SetRotation(direction);
	charge = 0;
	lifeFlag = true;
	return true;
}

void Player::Update(Camera* camera, AttackCollisionObject* AttackCol,Boss* boss,AudioManager* audio)
{
	PlayerFollowCamera(camera);
	StartAnimation(camera);
	SP->add(0.03f);
	moveDirection = cameraDirectionZ;
	if (ActionState == NONE || ActionState == WALK)
	{
		Walk(camera);
	}
	MoveCamera(camera,boss);
	DrillRush(AttackCol,audio);
	BlazingEnd(AttackCol,audio);
	Avoid(audio);
	UpdateWorldMatrix();
	MovementRangeLimit();
	Fall();
	UpdateWorldMatrix();
	collider->Update();
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);
	Object3d::Update();

	// クエリーコールバッククラス
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QueryHit& info) {

			const XMVECTOR up = { 0,1,0,0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			// 地面判定しきい値
			const float threshold = cosf(XMConvertToRadians(40.0f));

			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}
			return true;
		}

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
	};

	PlayerQueryCallback callback(sphereCollider);

	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	// 交差による排斥分動かす
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	BossHIt bossCallback(sphereCollider);

	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &bossCallback, COLLISION_ATTR_ENEMYS);

	KnockBack(boss, bossCallback,audio);

	UpdateWorldMatrix();
	collider->Update();

	IsGrounded(sphereCollider);
}

void Player::PlayerFollowCamera(Camera* camera)
{
	cameraPosition = camera->GetEye();
	cameraTarget = camera->GetTarget();

	/////////////////////////////////////////////////////////////////
	//⓵　現在のカメラの注視点と視点を使って、XZ平面上での、
	//注視点から視点までのベクトル(toCameraPosXZ)と長さ(toCameraPosXZLen)を求める。
	///////////////////////////////////////////////////////////////
	Vector3 toCameraPosXZ = cameraPosition - cameraTarget;
	float height = toCameraPosXZ.y;                  //視点へのY方向の高さは、後で使うのでバックアップしておく。
	toCameraPosXZ.y = 0.0f;                          //XZ平面にするので、Yは0にする。
	float toCameraPosXZLen = toCameraPosXZ.Length(); //XZ平面上での視点と注視点の距離を求める。
	toCameraPosXZ.Normalize();                       //単位ベクトル化。

	/////////////////////////////////////////////////////////////////
	// ⓶　新しい注視点をアクターの座標から決める。
	/////////////////////////////////////////////////////////////////
	target = GetPosition();
	target.y += distance;

	/////////////////////////////////////////////////////////////////
	// ⓷　新しい注視点と現在のカメラの視点を使って、XZ平面上での、
	//     注視点から視点までのベクトル(toNewCameraPos)を求める。
	/////////////////////////////////////////////////////////////////
	Vector3 toNewCameraPos = cameraPosition - target; //新しい注視点からカメラの始点へ向かうベクトルを求める。
	toNewCameraPos.y = 0.0f;              //XZ平面にするので、Yは0にする。
	toNewCameraPos.Normalize();         //単位ベクトル化。

	/////////////////////////////////////////////////////////////////
	// ⓸　１と２と３で求めた情報を使って、新しい視点を決定する。
	/////////////////////////////////////////////////////////////////
	//ちょっとづつ追尾。
	float weight = 0.4f;  //このウェイトの値は0.0～1.0の値をとる。1.0に近づくほど追尾が強くなる。
	toNewCameraPos = toNewCameraPos * weight + toCameraPosXZ * (1.0f - weight);
	toNewCameraPos.Normalize();
	toNewCameraPos *= toCameraPosXZLen;
	toNewCameraPos.y = height;              //高さを戻す。
	Vector3 pos = target + toNewCameraPos;  //これで新しい視点が決定。

	/////////////////////////////////////////////////////////////////
	// ⓹　視点と注視点をカメラに設定して終わり。
	/////////////////////////////////////////////////////////////////
	if (camera->GetDirtyFlag())
	{
		cameraTarget = target;
		camera->SetTarget(cameraTarget);
	}
}

void Player::MoveCamera(Camera* camera,Boss* boss)
{
	//prePos = position;
	//カメラのビュー行列の逆行列を計算
	camMatWorld = XMMatrixInverse(nullptr, camera->GetMatView());
	cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
	cameraDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();

	if (input->IsPush(DIK_RIGHT) || input->IsPush(DIK_LEFT))
	{
		//移動方向
		moveDirection = {};
		if (input->IsPush(DIK_RIGHT))
		{
			moveDirection += cameraDirectionX;
		}
		else if (input->IsPush(DIK_LEFT))
		{
			moveDirection += cameraDirectionX * -1;
		}
			
		moveDirection.Normalize();
		//回転処理
		//現在の進行方向とカメラの正面と角度を求める
		direction.Normalize();
		float cosA = direction.Dot(moveDirection);
		if (cosA > 0.1f)
			cosA = 1.0f;
		else if (cosA < -0.1f)
			cosA = -1.0f;

		float rotY = acos(cosA) * 180 / 3.14159365f;
		const Vector3 CrossVec = direction.Cross(moveDirection);

		float rotSpeed = rotateSpeed;
		if (rotSpeed > abs(rotY))
		{
			rotSpeed = rotY;
		}
		if (CrossVec.y < 0)
			rotSpeed *= -1;

		rotation.y += rotSpeed;
		XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(rotSpeed));
		XMVECTOR dir = { direction.x,direction.y,direction.z,0 };
		dir = XMVector3TransformNormal(dir, matRot);
		direction = dir;
	}
	else if(input->RelesePush(DIK_RIGHT) || input->RelesePush(DIK_LEFT))
	{
		rotCamera = true;
		float cosA = direction.Dot(cameraDirectionZ);
		if (cosA > 1.0f)
			cosA = 1.0f;
		else if (cosA < -1.0f)
			cosA = -1.0f;
		radY = acos(cosA);
		const Vector3 CrossVec = direction.Cross(cameraDirectionZ);
		if (CrossVec.y < 0)
			radY *= -1;
		cameraRotCount = 0;
	}
	//カメラの回転処理
	if (rotCamera)
	{
		cameraRotCount++;
		float rad = radY / rotTime;
		camera->AddPhi(rad);
		if (cameraRotCount >= rotTime)
			rotCamera = false;
	}
}

void Player::StartAnimation(Camera* camera)
{
	if (ActionState == STARTANIME)
	{
		camera->ResetCamera();
		PlayAnimation(0, 0);
		ActionState = NONE;
	}
}

void Player::Walk(Camera* camera)
{
	if (input->IsPush(DIK_A) || input->IsPush(DIK_D) || input->IsPush(DIK_W) || input->IsPush(DIK_S))
	{
		ActionState = WALK;
		moveDirection.Normalize();

		if (input->IsPush(DIK_A)) {
			moveDirection = cameraDirectionX * -1;
			position += moveDirection * walkSpeed;
		}

		if (input->IsPush(DIK_D)) {
			moveDirection = cameraDirectionX;
			position += moveDirection * walkSpeed;
		}

		if (input->IsPush(DIK_W)) {
			moveDirection = cameraDirectionZ;
			position += moveDirection * walkSpeed;
		}

		if (input->IsPush(DIK_S)) {
			moveDirection = cameraDirectionZ * -1;
			position += moveDirection * walkSpeed;
		}

		moveDirection.Normalize();
	}

	if (input->RelesePush(DIK_A) || input->RelesePush(DIK_D) || input->RelesePush(DIK_W) || input->RelesePush(DIK_S))
	{
		ActionState = NONE;
		vec = 0;
	}
}

void Player::Avoid(AudioManager* audio)
{
	if (input->TriggerPush(DIK_E) && !SP->Lessthan(1.0f) && ActionState == NONE ||
		input->TriggerPush(DIK_E) && !SP->Lessthan(1.0f) && ActionState == WALK)
	{
		PlayAnimation(3,0);
		audio->PlayWave(L"Resources/sounds/avoid.wav");
		DecisionDistance(cameraDirectionX * 30);
		SP->add(-1.0f);
		ActionState = AVOID;
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/movePlayer.efk", { GetPosition().x, GetPosition().y + 5, GetPosition().z });
		EffekseerManager::SetRotation(handle, {0,60,0});
	}
	else if (input->TriggerPush(DIK_Q) && !SP->Lessthan(1.0f) && ActionState == NONE ||
		input->TriggerPush(DIK_Q) && !SP->Lessthan(1.0f) && ActionState == WALK)
	{
		PlayAnimation(2,0);
		audio->PlayWave(L"Resources/sounds/avoid.wav");
		cameraDirectionX *= -1;
		DecisionDistance(cameraDirectionX * 30);
		SP->add(-1.0f);
		ActionState = AVOID;
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/movePlayer.efk", { GetPosition().x,  GetPosition().y + 5, GetPosition().z });
	}

	if (timerFlag && ActionState == AVOID)
	{
		const float avoidFrame = 0.7f;
		Easing(position, avoidFrame);
		AdvanceTimer(avoidFrame);
		
		if (IsEasingOver())
		{
			handle = EffekseerManager::StopEffect(handle);
			ResetAnimation();
			PlayAnimation(0, 0);
			ResetTimer();
			ActionState = NONE;
			if (input->RelesePush(DIK_E) || input->RelesePush(DIK_Q))
			{
				PlayAnimation(0, 0);
			}
		}
	}
}

void Player::DecisionDistance(Vector3 EashEnd)
{
	TimerStart();
	DashStart = { GetPosition().x,0 ,GetPosition().z };
	DashEnd = DashStart + EashEnd;
}

void Player::OnCollision(const CollisionInfo& info)
{
	//PlayAnimation();
	//DebugText::GetInstance()->Print("Collision detected",100,100,2.0f);
	//isPlay = false;
	////if (info.collider->GetShapeType() == COLLISIONSHAPE_MESH) return;
}

void Player::PositionCorrection(AttackCollisionObject* AttackCol,const Vector3 AddPos)
{
	AttColPos = AttackCol->GetPosition();
	AttColPos = position + AddPos;
	AttackCol->SetPosition(AttColPos);
	collisionFlag = AttackCol->GetColFlag();
	collisionFlag = !collisionFlag;
}

void Player::DrillRush(AttackCollisionObject* AttackCol, AudioManager* audio)
{
	if (input->TriggerPush(DIK_UP) && ActionState == NONE || input->TriggerPush(DIK_UP) && ActionState == WALK)
	{
		PlayAnimation(1, 0);
		audio->PlayWave(L"Resources/sounds/Drill.wav");
		DecisionDistance(moveDirection * 60);
		ActionState = ATTACK2;
		EffekseerManager::SetScale(handle,{5,5,5});
	}

	if (timerFlag && ActionState == ATTACK2)
	{
		const float AttackFlame2 = 2.09f;

		correctionPos = { 0,2,0 };
		Easing(position, AttackFlame2);
		PositionCorrection(AttackCol, GetCorrectionPos(2.0f, correctionPos));
		AdvanceTimer(AttackFlame2);

		if (IsEasingOver())
		{
			ResetTimer();
			ResetAnimation();
			PlayAnimation(0, 0);
			charge = 0;
			ActionState = NONE;
			collisionFlag = !collisionFlag;
		}
	}
}

void Player::Easing(Vector3& position,const float maxTime)
{
	position.x = Easing::EaseOutQuint(DashStart.x, DashEnd.x, maxTime, totalTime);

	position.z = Easing::EaseOutQuint(DashStart.z, DashEnd.z, maxTime, totalTime);
}

void Player::TimerStart()
{
	timerFlag = !timerFlag;

	if (time == nullptr)
	{
		time = new Timer();
	}
	else
	{
		time->restart();
	}
}

void Player::AdvanceTimer(const float maxTime)
{
	elapsedTime = time->elapsed();
	totalTime += elapsedTime;
	timeRate = min(totalTime / maxTime, 1.0f);
}

void Player::ResetTimer()
{
	timeRate = 0;
	elapsedTime = 0;
	totalTime = 0;
	timerFlag = !timerFlag;
}

bool Player::IsTimeOut(float TotalTime,const float Maxtime) const
{
	return TotalTime >= Maxtime;
}

bool Player::IsEasingOver() const
{
	const float EasingOver = 1.0f;//timeRateが1になるとEasing終了

	return timeRate == EasingOver;
}

void Player::BlazingEnd(AttackCollisionObject* AttackCol,AudioManager* audio)
{
	if (input->TriggerPush(DIK_DOWN) && ActionState == NONE || input->TriggerPush(DIK_DOWN) && ActionState == WALK)
	{
		PlayAnimation(4, 0);
		audio->PlayWave(L"Resources/sounds/swing1.wav");
		correctionPos = { 0,2,0 };
		PositionCorrection(AttackCol, GetCorrectionPos(6.0f, correctionPos));
		DecisionDistance(moveDirection * 30);
		ActionState = ATTACK3;
	}

	if (timerFlag && ActionState == ATTACK3)
	{
		const float AttackFlame3 = 1.6f;
		Easing(AttColPos, AttackFlame3);
		AttackCol->SetPosition(AttColPos);
		AdvanceTimer(AttackFlame3);
		
		if (IsEasingOver())
		{
			ResetAnimation();
			ResetTimer();
			PlayAnimation(0, 0);
			ActionState = NONE;
			collisionFlag = !collisionFlag;
		}
	}
}

void Player::KnockBack(Boss* boss, BossHIt bossCallback, AudioManager* audio)
{
	float knockBackDistance = 35.0f;
	Vector3 distination = position - boss->GetPosition();
	distination.Normalize();

	if (bossCallback.flag && ActionState == NONE || bossCallback.flag && ActionState == WALK)
	{
		PlayAnimation(5,0);
		audio->PlayWave(L"Resources/sounds/damage.wav");
		HP->add(-50);//ボスに触れた時ダメージ
		DecisionDistance(bossCallback.move + distination * knockBackDistance);
		ActionState = KNOCKBACK;
	}

	if (ActionState == KNOCKBACK)
	{
		const float AttackFlame3 = 2.5f;
		Easing(position, AttackFlame3);
		AdvanceTimer(AttackFlame3);

		if (IsEasingOver())
		{
			ResetAnimation();
			ResetTimer();
			PlayAnimation(0, 0);
			ActionState = NONE;

			//HPが0になったらゲームオーバー
			if (HP->isEmpty())
			{
				lifeFlag = !lifeFlag;
			}
		}
	}
}

void Player::RoarKnockBack(Boss* boss, bool roarFlag, AudioManager* audio)
{
	float knockBackDistance = 35.0f;
	Vector3 distination = position - boss->GetPosition();
	distination.Normalize();
	float roarMove = 1.1f;

	if (ActionState == NONE && roarFlag || ActionState == WALK && roarFlag)
	{
		audio->PlayWave(L"Resources/sounds/damage.wav");
		PlayAnimation(5, 0);
		HP->add(-50);//ボスに触れた時ダメージ
		DecisionDistance(distination * knockBackDistance);
		ActionState = KNOCKBACK;
	}

	if (ActionState == KNOCKBACK)
	{
		const float AttackFlame3 = 1.5f;
		Easing(position, AttackFlame3);
		AdvanceTimer(AttackFlame3);

		if (IsEasingOver())
		{
			ResetAnimation();
			ResetTimer();
			PlayAnimation(0, 0);
			ActionState = NONE;
			boss->GetRoarFlag() = false;

			//HPが0になったらゲームオーバー
			if (HP->isEmpty())
			{
				lifeFlag = !lifeFlag;
			}
		}
	}
}

void Player::MovementRangeLimit()
{
	/// 移動境界線
	const int range = 550;
	if (position.x >= range)
	{
		position.x = range;
	}
	else if (position.x <= -range)
	{
		position.x = -range;
	}

	if (position.z >= range)
	{
		position.z = range;
	}
	else if (position.z <= -range)
	{
		position.z = -range;
	}
}

void Player::Fall()
{
	//// 落下処理
	if (!onGround && ActionState == RUN || !onGround && ActionState == NONE
		|| !onGround && ActionState == WALK) {
		// 下向き加速度
		const float fallAcc = -0.07f;
		const float fallVYMin = -0.75f;
		// 加速
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		// 移動
		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}
}

void Player::IsGrounded(SphereCollider* sphereCollider)
{
	// 球の上端から球の下端までのレイキャスト
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	// 接地状態
	if (onGround) {
		// スムーズに坂を下る為の吸着距離
		const float adsDistance = 0.2f;
		// 接地を維持
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			// 行列の更新など
			Object3d::Update();
		}
		// 地面がないので落下
		else {
			onGround = false;
			fallV = {};
		}
	}
	// 落下状態
	else if (fallV.m128_f32[1] <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f)) {
			// 着地
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			// 行列の更新など
			Object3d::Update();
		}
	}
}

void Player::ResetCamera(Boss* boss, Camera* camera)
{
	float cosA = direction.Dot(cameraDirectionZ);
	if (cosA > 1.0f)
		cosA = 1.0f;
	else if (cosA < -1.0f)
		cosA = -1.0f;
	radY = acos(cosA);
	const Vector3 CrossVec = direction.Cross(boss->GetPosition());
	if (CrossVec.y < 0)
		radY *= -1;
	
	float rad = radY / rotTime;
	camera->AddPhi(rad);
}

bool Player::BossHIt::OnQueryHit(const QueryHit& info)
{
	const XMVECTOR up = { 0,1,0,0 };

	XMVECTOR rejectDir = XMVector3Normalize(info.reject);
	float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

	// 地面判定しきい値
	const float threshold = cosf(XMConvertToRadians(40.0f));

	if (-threshold < cos && cos < threshold) {
		sphere->center += info.reject;
		move += info.reject;
	}

	flag = true;

	return true;
}
