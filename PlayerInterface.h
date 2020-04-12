#pragma once

class PlayerInterface
{
public:
	PlayerInterface() = default;
	virtual ~PlayerInterface() = default;

	virtual void DestroySelf() = 0; 
};