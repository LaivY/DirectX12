#pragma once
#include "stdafx.h"
#include "object.h"
#include "terrain.h"

#define ROLL_MAX +20
#define ROLL_MIN -10

class Camera;

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void AddVelocity(const XMFLOAT3& increase);
	void SetVelocity(const XMFLOAT3& velocity) { m_velocity = velocity; }
	void SetCamera(const shared_ptr<Camera>& camera) { m_camera = camera; }
	void SetTerrain(HeightMapTerrain* terrain) { m_terrain = terrain; }

	XMFLOAT3 GetVelocity() const { return m_velocity; }
	HeightMapTerrain* GetTerrain() const { return m_terrain; }

private:
	XMFLOAT3			m_velocity;		// �ӵ�
	FLOAT				m_maxVelocity;	// �ִ�ӵ�
	FLOAT				m_friction;		// ������
	shared_ptr<Camera>	m_camera;		// ī�޶�
	HeightMapTerrain*	m_terrain;		// �÷��̾ ���ִ� ����
};