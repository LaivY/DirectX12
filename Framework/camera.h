#pragma once
#include "stdafx.h"
#include "player.h"

#define MAX_ROLL +20
#define MIN_ROLL -10

class Camera
{
public:
	Camera();
	~Camera() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void UpdateLocalAxis();
	virtual void UpdatePosition(FLOAT deltaTime) { };

	void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	XMFLOAT4X4 GetViewMatrix() const { return m_viewMatrix; }
	XMFLOAT4X4 GetProjMatrix() const { return m_projMatrix; }
	void SetViewMatrix(const XMFLOAT4X4& viewMatrix) { m_viewMatrix = viewMatrix; }
	void SetProjMatrix(const XMFLOAT4X4& projMatrix) { m_projMatrix = projMatrix; }

	XMFLOAT3 GetEye() const { return m_eye; }
	XMFLOAT3 GetAt() const { return m_at; }
	XMFLOAT3 GetUp() const { return m_up; }
	void SetEye(const XMFLOAT3& eye) { m_eye = eye; UpdateLocalAxis(); }
	void SetAt(const XMFLOAT3& at) { m_at = at; UpdateLocalAxis(); }
	void SetUp(const XMFLOAT3& up) { m_up = up; UpdateLocalAxis(); }

	XMFLOAT3 GetU() const { return m_u; }
	XMFLOAT3 GetV() const { return m_v; }
	XMFLOAT3 GetN() const { return m_n; }

	shared_ptr<Player> GetPlayer() const { return m_player; }
	void SetPlayer(const shared_ptr<Player>& player);

protected:
	// ��, ���� ��ȯ ���
	XMFLOAT4X4			m_viewMatrix;
	XMFLOAT4X4			m_projMatrix;

	// ī�޶� �Ķ���� 3���
	XMFLOAT3			m_eye;
	XMFLOAT3			m_at; // eye���� �ٶ󺸴� ���� ����
	XMFLOAT3			m_up;

	// ī�޶� ���� x, y, z��
	XMFLOAT3			m_u;
	XMFLOAT3			m_v;
	XMFLOAT3			m_n;

	// ȸ����
	FLOAT				m_roll;
	FLOAT				m_pitch;
	FLOAT				m_yaw;

	// ������ ������
	FLOAT				m_delay;

	// �÷��̾�
	shared_ptr<Player>	m_player;
};

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera();
	~ThirdPersonCamera() = default;

	virtual void UpdatePosition(FLOAT deltaTime);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	XMFLOAT3 GetOffset() const { return m_offset; }
	void SetOffset(const XMFLOAT3& offset) { m_offset = offset; }
	void SetDelay(FLOAT delay) { m_delay = delay; }

private:
	XMFLOAT3	m_offset;
	FLOAT		m_delay;
};