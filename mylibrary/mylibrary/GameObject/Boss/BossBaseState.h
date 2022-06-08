#pragma once
#include "Boss.h"
#include "../mylibrary/GameObject/Player/Player.h"
#include "../mylibrary/Base/Timer.h"
#include "../mylibrary/3d/AttackEnemyCollisionObject.h"
#include "../mylibrary/Audio/AudioManager.h"
#include "../mylibrary/Base/HelperTimer.h"

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

	BossStatePattern GetStatePattern() { return statePattern; }
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
	Vector3 CalculatedPos;//�v���C���[�̍��W����{�X�̍��W�����������W

	const unsigned int STATE_COUNT = (unsigned int)BossStatePattern::END;
	const unsigned int NEAR_BEGIN = (unsigned int)BossStatePattern::BITING;
	const unsigned int ANYFIRST = (unsigned int)BossStatePattern::LOCKON;
	const unsigned int FAR_BEGIN = (unsigned int)BossStatePattern::RUSH;
	const unsigned int CHENGEMAXTIME = 600.0f;

	float angle;
	float velocity;//���x
	float maxTime;   //�S�̎���[s]
	float distance; //����
	bool collisionFlag;
};