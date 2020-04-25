#include "SimpleAI.h"
#include "PlayerInterface.h"
#include "Transform.h"
#include "Entity.h"
#include <DirectXMath.h>

#include <cstdio>

using namespace DirectX;

SimpleAI::SimpleAI(class PlayerInterface* pPlayer, class Entity** path, class Entity* pSelf)
{
	player = pPlayer;
	targetPath = path;
	self = pSelf;
	maxRouteCount = 4;
	activeRoute = 0;
	ghostSpeedBoost = 3.f;
	state = AI_State::PATROL_PATH;
}

void SimpleAI::Update(bool inLight, float deltaTime)
{
	UpdateState(inLight);

	switch(state)
	{
	case AI_State::PATROL_PATH:
		ExecutePatrolPath(deltaTime);
		break;

	case AI_State::ATTACK_PLAYER:
		ExecuteAttackPlayer(deltaTime);
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
		AIMoveTowards(activePath->GetTransform(), deltaTime);

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

// Behavior for following the player
void SimpleAI::ExecuteAttackPlayer(float deltaTime)
{
	AIMoveTowards(player->GetTransform(), deltaTime);

	// @todo check if hit player, if so, send signal to end game
}

void SimpleAI::UpdateState(bool playerInLight)
{	
	// @note: the ghost's visibility range could be implemented as a member, 
	// but this is only useful if we want to vary the ghost's vision range
	const float sqLightRange = 9.0f * 9.0f;
	const float sqDarkRange  = 6.0f * 6.0f;

	// squared distance to player
	float sqDist = self->GetTransform()->DistanceSquaredTo(player->GetTransform()->GetPosition());
	
	float range = -1.0f;
	// Ghosts can see farther if player is in light
	if (playerInLight)
		range = sqLightRange;
	else
		range = sqDarkRange;

	if (sqDist < range) // Player spotted
	{
		#ifdef DEBUG // State has changed from passive->attacking
		if(state == AI_State::PATROL_PATH)
			printf("Attack %d!\n", playerInLight);
		#endif
		
		SetState(AI_State::ATTACK_PLAYER);
	}
	else // Player lost
	{
		#ifdef DEBUG // State has changed from attacking->passive
		if(state == AI_State::ATTACK_PLAYER)
			printf("Back to work.\n");
		#endif

		SetState(AI_State::PATROL_PATH);
	}
}

void SimpleAI::AIMoveTowards(Transform* pTarget, float deltaTime)
{
	Transform* ghostTransform = self->GetTransform();

	// Both positions as XMVECTOR
	XMVECTOR targetPos = XMLoadFloat3(&pTarget->GetPosition());
	XMVECTOR ghostPos = XMLoadFloat3(&ghostTransform->GetPosition());
	
	// SIMD operations
	XMVECTOR dir = XMVectorSubtract(targetPos, ghostPos);
	XMVECTOR dirNorm = XMVector3Normalize(dir);

	// Adjust relative to deltaTime
	dirNorm *= deltaTime * ghostSpeedBoost;
	
	// Call Transform movement method
	XMFLOAT3 dirFl;
	XMStoreFloat3(&dirFl, dirNorm);
	ghostTransform->MoveAbsolute(dirFl.x, 0, dirFl.z);
}

