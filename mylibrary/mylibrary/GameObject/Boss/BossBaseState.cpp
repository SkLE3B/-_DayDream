#include "BossBaseState.h"

void BossBaseState::Initialize()
{
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	timerFlag = false;
	collisionFlag = false;
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
	position.x = Easing::EaseOutQuart(EaseStart.x, EaseEnd.x, maxTime, totalTime);

	position.z = Easing::EaseOutQuart(EaseStart.z, EaseEnd.z, maxTime, totalTime);
}

void BossBaseState::EasingRot(Vector3& position, const float maxTime, Player* player, Vector3 rot)
{
	angle = atan2f(Ppos.x - position.x, Ppos.z - position.z) * 57;

	rot.y = Easing::EaseOutQuint(REaseStart.y, angle, maxTime, totalTime);

	weak_boss.lock()->SetRotation(rot);
}

void BossBaseState::EasingRolling(const float maxTime,Vector3 rot)
{
	angle = REaseStart.y + 360;
	rot.y = Easing::EaseOutQuint(REaseStart.y, angle, maxTime, totalTime);
	weak_boss.lock()->SetRotation(rot);
}

void BossBaseState::TimerStart(Timer* time, bool* timerFlag)
{
	*timerFlag = !*timerFlag;

	if (time == nullptr)
	{
		time = new Timer();
	}
	else
	{
		time->restart();
	}
}

void BossBaseState::AdvanceTimer(const float maxTime)
{
	elapsedTime = time.elapsed();
	totalTime += elapsedTime;
	timeRate = min(totalTime / maxTime, 1.0f);
}

void BossBaseState::ResetTimer()
{
	timeRate = 0;
	elapsedTime = 0;
	totalTime = 0;
	EaseStart = { 0,0,0 };
	EaseEnd = { 0,0,0 };
	timerFlag = !timerFlag;
}

bool BossBaseState::IsTimeOut(float TotalTime, const float Maxtime) const
{
	return TotalTime >= Maxtime;
}

bool BossBaseState::IsEasingOver() const
{
	const float EasingOver = 1.0f;//timeRate‚ª1‚É‚È‚é‚ÆEasingI—¹

	return timeRate == EasingOver;
}