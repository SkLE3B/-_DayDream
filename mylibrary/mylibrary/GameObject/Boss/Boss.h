#pragma once
#include "3d/Object3d.h"
#include "Math/Easing.h"
#include "Input/Input.h"
#include "Player.h"
#include "Collision/SphereCollider.h"
#include "3d/AttackEnemyCollisionObject.h"
#include "Math/Matrix4.h"
#include "BossBaseState.h"
#include "Audio/AudioManager.h"

class BossBaseState;
class AttackEnemyCollisionObject;
class Player;
class Boss :
    public Object3d, public std::enable_shared_from_this<Boss>
{
public://�G�C���A�X
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;
	using shared_ptrState = std::shared_ptr<BossBaseState>;

public:
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	//static Boss* Create(Model* model = nullptr);
	static std::shared_ptr<Boss> Create(Model* model = nullptr);
public:
	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update(Player* player, AttackEnemyCollisionObject* AttackCollision, AudioManager* audio);

	/// <summary>
    /// �␳���W���Z�b�g����
    /// </summary>
    /// <param name="distance">����</param>
    /// <param name="correctionPosY">�␳����Y���W</param>
	Vector3 GetCorrectionPos(const float& distance, Vector3& correctionPosY)
	{
		return ((atan2f(player_position.x - position.x, player_position.z - position.z) * 60) * distance) + correctionPosY;
	}

	//SetterGetter
	float& GetTimeRate() { return timeRate; }
	float& GetTime();
	float& GetAngle() { return angle; }
	Vector3& Getresult() { return moveVector_position; }

	float& GetDotVector() { return dotPosition; }
	float& GetradiusCos() { return radiusCos; }
	float GetAngle(int mx, int my, int sx, int sy) {
		return short(atan2f(-float(sy - my), float(sx - mx)));
	}

	SphereCollider& GetSphereCollider() { return *sphereCollider;}

	bool& GetRoarFlag();
	bool& GetColFlag();
	void ChangeState(std::shared_ptr<BossBaseState> weak_bossState);
	bool ChangeLifeFlag() { return lifeFlag = !lifeFlag; }
	bool GetLifeFlag() { return lifeFlag;}

	shared_ptrState GetState() { return shared_bossState; }
private:
	std::shared_ptr<BossBaseState> shared_bossState;
	Input*          input = Input::GetInstance();   // �}�E�X�̓��͂��擾
	Vector3         moveVector_position;            //�ړ��ʂ̃x�N�g�����W
	Vector3         player_position;                //�v���C���[�̍��W
	Vector3         boss_position;                  //�{�X�̍��W
	Vector3         easing_start;                   //�C�[�W���O�J�n���W
	Vector3         rotationEasing_start;           //��]�C�[�W���O�J�n���W
	Vector3         easing_end;                     //�C�[�W���O�I�����W
	Vector3         attackObject_position;          //�U���I�u�W�F�N�g���W
	SphereCollider* sphereCollider;                 //���R���C�_�[
	Vector3         forwardVector;                  //�O�����x�N�g��
	Matrix4         warldMatrix;                    //���[���h���W�s��
	Vector3         walrdPosition;                  //���[���h���W
	Vector3         differencePlayerToBoss_position;//�v���C���[���W����{�X���W�̍�
	float           dotPosition;                    //����
	float           radiusCos;                      //���a�̃R�T�C��
	float           maxTime;                        //�S�̎���[s]
	float           timeRate;                       //�������Ԃ��i�񂾂�(��)
	float           elapsedTime;                    //�o�ߎ���
	float           totalTime;                      //�i�߂�����
	bool            timerFlag;                      //�^�C�}�[���N�����Ă��邩�ǂ���
	float           angle;                          //�p�x
	float           distanceMagnification;          //�����̔{��
	bool            lifeFlag;                       //���C�t�t���O
	float           radius;                         //���a
	Vector3         angleVector;                    //�p�x�x�N�g��
};

