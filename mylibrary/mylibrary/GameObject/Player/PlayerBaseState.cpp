#include "PlayerBaseState.h"

void PlayerBaseState::DecisionDistance(Vector3 EashEnd)
{
	Start = { weak_player.lock()->GetPosition().x,0 ,weak_player.lock()->GetPosition().z };
	End = Start + EashEnd;
}

void PlayerBaseState::Easing(Vector3& position, const float maxTime)
{
	position.x = Easing::EaseOutQuint(Start.x, End.x, maxTime, timer.GetCurrentTimer());

	position.z = Easing::EaseOutQuint(Start.z, End.z, maxTime, timer.GetCurrentTimer());
}

void PlayerBaseState::PositionCorrection(AttackCollisionObject* AttackCol, const Vector3 AddPos)
{
	Vector3 AttColPos = weak_player.lock()->GetPosition() + AddPos;
	AttackCol->SetPosition(AttColPos);
}