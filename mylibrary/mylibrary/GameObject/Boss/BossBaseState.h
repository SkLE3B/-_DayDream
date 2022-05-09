#pragma once
#include "Boss.h"
#include "Player.h"
#include "Base/Timer.h"
#include "3d/AttackEnemyCollisionObject.h"
#include "Audio/AudioManager.h"

enum class BossStatePattern
{
	WAIT,
	LOCKON,
	BACKSTEP,
	ROLLING, 
	BITING,  
	ROAR,    
	BRETH,   
	RUSH,    
	END
};

class AttackEnemyCollisionObject;
class Player;
class BossBaseState
{
public://�G�C���A�X
	using weak_ptrBoss = std::weak_ptr<Boss>;
	using weak_ptrPlayer = std::weak_ptr<Player>;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	BossBaseState() {}
	virtual ~BossBaseState() {}

	virtual void Initialize() = 0;
	virtual void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio) = 0;
	virtual void Finalize() = 0;

	/// <summary>
    /// �n�_�ƏI�_�̍��W��ۑ�����
    /// </summary>
    /// <param name="DashX">�I�_X���W</param>
    /// <param name="DashZ">�I�_Z���W</param>
    /// <param name="Flag"> �ς������t���O</param>
	void DecisionDistance(const float EaseEndX, const float EaseEndZ);

	/// <summary>
	/// �����蔻���␳����
	/// </summary>
	/// <param name="AttackCol">�����蔻��</param>
	/// <param name="AddPos">��������W</param>
	void PositionCorrection(AttackEnemyCollisionObject* AttackCol, const Vector3 AddPos);

	/// <summary>
	/// �ړ��C�[�W���O
	/// </summary>
	/// <param name="position">���������W</param>
	/// <param name="maxTime">�ړ�����</param>
	void EasingMove(Vector3& position, const float maxTime, Player* player);

	 //<summary>
	 //��]�C�[�W���O
	 //</summary>
	 //<param name="position"></param>
	 //<param name="maxTime"></param>
	 //<param name="player"></param>
	void EasingRot(Vector3& position, const float maxTime, Player* player,Vector3 rot);

	/// <summary>
	/// ��]�C�[�W���O(��]�U���p)
	/// </summary>
	/// <param name="position"></param>
	/// <param name="maxTime"></param>
	/// <param name="player"></param>
	void EasingRolling(const float maxTime,Vector3 rot);

	//<summary>
    //�^�C�}�[���N������
    //</summary>
	void TimerStart(Timer* time, bool* timerFlag);

	/// <summary>
	/// �^�C�}�[��i�߂�
	/// </summary>
	/// <param name="maxTime"></param>
	void AdvanceTimer(const float maxTime);

	/// <summary>
	/// �^�C�}�[���~�߂�
	/// </summary>
	void ResetTimer();

	/// <summary>
	/// �^�C�}�[��MaxTime�ɓ��B�������ǂ���
	/// </summary>
	/// <param name="TotalTime">�J�E���g���Ă���^�C�}�[</param>
	/// <param name="Maxtime">�I���t���[��</param>
	/// <returns></returns>
	bool IsTimeOut(float TotalTime, const float Maxtime) const;

	/// <summary>
	/// �C�[�W���O���I��������ǂ���
	/// </summary>
	/// <returns></returns>
	bool IsEasingOver() const;

	BossStatePattern GetStatePattern() { return statePattern; }
	float& GetTime() { return totalTime; }
	bool& GetRoarFlag() { return roarFlag; }
	bool& GetCollisionFlag() { return collisionFlag; }

	void SetBoss(std::weak_ptr<Boss> weakBoss) { weak_boss = weakBoss; }
protected:
	weak_ptrBoss weak_boss;
	weak_ptrPlayer player;
	BossStatePattern statePattern;
	Timer time;

	Vector3 EaseStart;//�C�[�W���O�J�n���W
	Vector3 REaseStart;//�C�[�W���O�J�n���W
	Vector3 EaseEnd;  //�C�[�W���O�I�����W
	Vector3 Ppos;     //�v���C���[�̍��W
	Vector3 rotboss;  //��]
	Vector3 AttackPos;//�U�������蔻����W

	Vector3 forwardVector;//�O�����x�N�g��
	Matrix4 warldMat;//���[���h���W�s��
	Vector3 walrdPos;//���[���h���W�x�N�g��
	Vector3 walrdPos2;//���[���h���W�x�N�g��
	Vector3 sa;

	const unsigned int STATE_COUNT = (unsigned int)BossStatePattern::END;
	const unsigned int NEAR_BEGIN = (unsigned int)BossStatePattern::BITING;
	const unsigned int ANYFIRST = (unsigned int)BossStatePattern::LOCKON;
	const unsigned int FAR_BEGIN = (unsigned int)BossStatePattern::RUSH;
	const unsigned int CHENGEMAXTIME = 600.0f;

	float maxTime;   //�S�̎���[s]
	float timeRate;  //�������Ԃ��i�񂾂�(��)
	float elapsedTime;//�������Ԃ��i�񂾂�(�l)
	float totalTime;  //�i�߂�����
	bool timerFlag;    //�^�C�}�[���N�����Ă��邩�ǂ���
	float angle;
	float bai; //����
	bool roarFlag;
	bool collisionFlag;
};