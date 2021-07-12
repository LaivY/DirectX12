#pragma once
#include "stdafx.h"
#include "object.h"

class Camera;

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

	XMFLOAT3 GetVelocity() const { return m_velocity; }
	void SetVelocity(const XMFLOAT3& velocity) { m_velocity = velocity; }
	void AddVelocity(const XMFLOAT3& increase);
	void SetCamera(const shared_ptr<Camera>& camera) { m_camera = camera; }
	void ApplyFriction() { m_velocity = Vector3::Mul(m_velocity, 1.0f - m_friction); }

private:
	XMFLOAT3			m_velocity;		// �ӵ�
	XMFLOAT3			m_maxVelocity;	// �ִ�ӵ�
	FLOAT				m_friction;		// ������(0.0 ~ 1.0)
	shared_ptr<Camera>	m_camera;		// ī�޶�
};