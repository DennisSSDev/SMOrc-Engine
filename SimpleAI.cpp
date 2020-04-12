#include "SimpleAI.h"
#include "PlayerInterface.h"
#include "Transform.h"
#include "Entity.h"
#include <DirectXMath.h>

using namespace DirectX;

SimpleAI::SimpleAI(class PlayerInterface* pPlayer, class Entity** path, class Entity* pSelf)
{
	player = pPlayer;
	targetPath = path;
	self = pSelf;
	maxRouteCount = 4;
	activeRoute = 0;
	ghostSpeedBoost = 5.f;
	state = AI_State::PATROL_PATH;
}

void SimpleAI::Update(float deltaTime)
{
	switch(state)
	{
	case AI_State::PATROL_PATH:
		ExecutePatrolPath(deltaTime);
		break;
	default:
		break;
	}
}

void SimpleAI::ExecutePatrolPath(float deltaTime)
{
	Transform* ghostTransform = self->GetTransform();
	Entity* activePath = targetPath[activeRoute];
	if (ghostTransform->DistanceSquaredTo(activePath->GetTransform()->GetPosition()) > 1.001f)
	{
		XMFLOAT3 routePos = activePath->GetTransform()->GetPosition();
		XMFLOAT3 ghostPos = ghostTransform->GetPosition();

		XMVECTOR ghostPosSIMD = XMLoadFloat3(&ghostPos);
		XMVECTOR routePosSIMD = XMLoadFloat3(&routePos);
		XMVECTOR dir = XMVectorSubtract(routePosSIMD, ghostPosSIMD);
		XMVECTOR dirNorm = XMVector3Normalize(dir);
		dirNorm *= deltaTime * ghostSpeedBoost;
		XMFLOAT3 dirFl;
		XMStoreFloat3(&dirFl, dirNorm);
		ghostTransform->MoveAbsolute(dirFl.x, 0, dirFl.z);

		// @todo one day we will make them face the target that they want to attack.
		// XMVECTOR ghostQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&ghostTransform->GetPitchYawRoll()));
	}
	else
	{
		if (activeRoute >= maxRouteCount)
		{
			activeRoute = 0;
		}
		else
		{
			activeRoute++;
		}
	}
}

void SimpleAI::ExecuteAttackPlayer()
{
	// @todo
}

