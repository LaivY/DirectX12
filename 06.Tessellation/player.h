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

	void SetPlayerOnTerrain();

	void AddVelocity(const XMFLOAT3& increase);
	void SetVelocity(const XMFLOAT3& velocity) { m_velocity = velocity; }
	void SetCamera(const shared_ptr<Camera>& camera) { m_camera = camera; }
	void SetTerrain(HeightMapTerrain* terrain) { m_terrain = terrain; }

	XMFLOAT3 GetVelocity() const { return m_velocity; }
	HeightMapTerrain* GetTerrain() const { return m_terrain; }

private:
	XMFLOAT3			m_velocity;		// 속도
	FLOAT				m_maxVelocity;	// 최대속도
	FLOAT				m_friction;		// 마찰력
	shared_ptr<Camera>	m_camera;		// 카메라
	HeightMapTerrain*	m_terrain;		// 플레이어가 서있는 지형
};