#include "BossBaseState.h"

void BossBaseState::Initialize()
{
	maxTime = 0.3f;//‘S‘ÌŽžŠÔ[s]
	angle = 0;
	Ppos = {};
	EaseStart = {};
	EaseEnd = {};
	forwardVector = { 0,0,1 };
}

void BossBaseState::DecisionDistance(const float EaseEndX, const float EaseEndZ)
{
	EaseStart = { weak_boss.lock()->GetPosition().x,0 , weak_boss.lock()->GetPosition().z };
	REaseStart = { 0,  weak_boss.lock()->GetRotation().y, 0 };
	EaseEnd = { EaseEndX, 0, EaseEndZ };
}

void BossBaseState::PositionCorrection(AttackEnemyCollisionObject* AttackCol, const Vector3 AddPos)
{
	AttackPos = AttackCol->GetPosition();
	AttackPos = weak_boss.lock()->GetPosition() + AddPos;
	AttackCol->SetPosition(AttackPos);
}

void BossBaseState::EasingMove(Vector3& position, const float maxTime, Player* player)
{
	position.x = Easing::EaseOutQuart(EaseStart.x, EaseEnd.x, maxTime, time.GetCurrentTimer());

	position.z = Easing::EaseOutQuart(EaseStart.z, EaseEnd.z, maxTime, time.GetCurrentTimer());
}

void BossBaseState::EasingRot(Vector3& position, const float maxTime, Player* player, Vector3 rot)
{
	angle = atan2f(Ppos.x - position.x, Ppos.z - position.z) * 57;

	rot.y = Easing::EaseOutQuint(REaseStart.y, angle, maxTime, time.GetCurrentTimer());

	weak_boss.lock()->SetRotation(rot);
}

void BossBaseState::EasingRolling(const float maxTime,Vector3 rot)
{
	angle = REaseStart.y + 360;
	rot.y = Easing::EaseOutQuint(REaseStart.y, angle, maxTime, time.GetCurrentTimer());
	weak_boss.lock()->SetRotation(rot);
}