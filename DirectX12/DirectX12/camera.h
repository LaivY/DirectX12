#pragma once
#include "stdafx.h"
#include "player.h"

#define MAX_ROLL +10
#define MIN_ROLL -10

class Camera
{
public:
	Camera();
	~Camera() = default;

	void Update(const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void UpdateLocalAxis();
	void Move(const XMFLOAT3& shift);
	void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

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

	Player* GetPlayer() const { return m_player.get(); }
	void SetPlayer(const shared_ptr<Player>& player);

	XMFLOAT3 GetOffset() const { return m_offset; }
	void SetOffset(const XMFLOAT3& offset) { m_offset = offset; }

private:
	// 뷰, 투영 변환 행렬
	XMFLOAT4X4			m_viewMatrix;
	XMFLOAT4X4			m_projMatrix;

	// 카메라 파라미터 3요소
	XMFLOAT3			m_eye;
	XMFLOAT3			m_at; // eye에서 바라보는 방향 벡터
	XMFLOAT3			m_up;

	// 카메라 로컬 x, y, z축
	XMFLOAT3			m_u;
	XMFLOAT3			m_v;
	XMFLOAT3			m_n;

	// 회전각
	FLOAT				m_roll;
	FLOAT				m_pitch;
	FLOAT				m_yaw;

	// 움직임 딜레이
	FLOAT				m_delay;

	// 플레이어, 오프셋
	shared_ptr<Player>	m_player;
	XMFLOAT3			m_offset;
};