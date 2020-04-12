#pragma once

class Entity;
class PlayerInterface;

enum class AI_State: unsigned char
{
	DEFAULT = 0x01,
	PATROL_PATH = 0x02,
	ATTACK_PLAYER = 0x04
};

class SimpleAI 
{
public:
	SimpleAI(class PlayerInterface* pPlayer, class Entity** path, class Entity* pSelf);
	~SimpleAI() = default;

	inline void SetState(AI_State pState) {state = pState;}
	virtual void Update(float deltaTime);

private:
	void ExecutePatrolPath(float deltaTime);
	void ExecuteAttackPlayer();

	class Entity** targetPath = nullptr;
	class PlayerInterface* player = nullptr;
	class Entity* self = nullptr;

	AI_State state = AI_State::DEFAULT;
	size_t activeRoute;
	size_t maxRouteCount;

	float ghostSpeedBoost;
};