#include "Player.h"
#include "../mylibrary/Input/Input.h"
#include "../mylibrary/2d/DebugText.h"
#include "../mylibrary/Collision/SphereCollider.h"
#include "../mylibrary/3d/ParticleManager.h"
#include "../mylibrary/Collision/CollisionManager.h"
#include "../mylibrary/Collision/CollisionAttribute.h"
#include "../mylibrary/Input/Gamepad.h"
#include "../mylibrary/Collision/MeshCollider.h"
#include "../mylibrary/Camera/DebugCamera.h"
#include "../mylibrary/Math/Easing.h"
#include "PlayerState_None.h"
#include "time.h"
#include <windows.h>
#pragma comment(lib, "winmm.lib")

using namespace DirectX;

Camera* Player::camera = nullptr;
Player::BossHIt Player::bossCallback = nullptr;
bool Player::BossHIt::flag = false;

std::shared_ptr<Player> Player::Create(Model* model, Camera* camera)
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
	direction = { 0,0,1 };
	cameraRotCount = 0;
	rotCamera = false;
	radY = 0;
	// コライダーの追加
	radius = 2.0f;
	SetCollider(new SphereCollider(XMVECTOR({0,radius,0,0 }), radius),COLLISION_ATTR_ALLIES);
    distance = 10.0f;
	HP = new ExNum(800.0f, 0.0f, 800.0f);

	cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
	cameraDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();
	moveDirection = 0;
	cameraTarget = { 0,1,0 };
	rotation = { 0,0,0 };
	SetRotation(direction);
	ChangeState(std::make_shared<PlayerState_None>());
	lifeFlag = true;
	camera->ResetCamera();
	return true;
}

void Player::Update(Camera* camera, AttackCollisionObject* AttackCol,Boss* boss,AudioManager* audio)
{
	PlayerFollowCamera(camera);
	moveDirection = cameraDirectionZ;

	MoveCamera(camera,boss);
	UpdateWorldMatrix();
	MovementRangeLimit();
	Fall();
	UpdateWorldMatrix();
	collider->Update();
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);
	Object3d::Update();

	if (shared_PlayerState)
	{
		shared_PlayerState->Update(camera,AttackCol,boss,audio);
	}

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

	bossCallback = dynamic_cast<SphereCollider*>(collider);
	// 球と地形の交差を全検索
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &bossCallback, COLLISION_ATTR_ENEMYS);

	UpdateWorldMatrix();
	collider->Update();

	IsGrounded(sphereCollider);
}

void Player::ChangeState(std::shared_ptr<PlayerBaseState> weak_PlayerState)
{
	if (shared_PlayerState)
	{
		shared_PlayerState->Finalize();
	}

	shared_PlayerState = weak_PlayerState;
	shared_PlayerState->SetPlayer(shared_from_this());
	shared_PlayerState->Initialize();
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
	float weight = 0.0f;  //このウェイトの値は0.0～1.0の値をとる。1.0に近づくほど追尾が強くなる。
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
		camera->SetEye(cameraPosition);
	}
}

void Player::MoveCamera(Camera* camera,Boss* boss)
{
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

void Player::OnCollision(const CollisionInfo& info)
{
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
	if (!onGround) {
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