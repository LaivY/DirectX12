#pragma once
#include "stdafx.h"
#include "object.h"

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

private:
	FLOAT m_velocity;
};