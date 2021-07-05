#include "camera.h"

Camera::Camera() :
	m_eye{ 0.0f, 0.0f, 0.0f }, m_at{ 0.0f, 0.0f, 1.0f }, m_up{ 0.0f, 1.0f, 0.0f },
	m_u{ 1.0f, 0.0f, 0.0f }, m_v{ 0.0f, 1.0f, 0.0f }, m_n{ 0.0f, 0.0f, 1.0f },
	m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }, m_delay{ 0.0f }, m_offset{ 0.0f, 0.0f, 0.0f }
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMatrix, XMMatrixIdentity());
}

void Camera::Update(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	// ī�޶� �� ��ȯ ��� �ֽ�ȭ
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&Vector3::Add(m_eye, m_at)), XMLoadFloat3(&m_up)));
	
	// DIRECTX�� ��켱(row-major), HLSL�� ���켱(column-major)
	// ����� ���̴��� �Ѿ �� �ڵ����� ��ġ ��ķ� ��ȯ�ȴ�.
	// �׷��� ���̴��� ��ġ ����� �Ѱ��ָ� DIRECTX�� ���� ������ �����ϰ� ����� �� �ִ�.
	XMFLOAT4X4 transViewMatrix, transProjMatrix;
	XMStoreFloat4x4(&transViewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix)));
	XMStoreFloat4x4(&transProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_projMatrix)));

	commandList->SetGraphicsRoot32BitConstants(1, 16, &transViewMatrix, 0);
	commandList->SetGraphicsRoot32BitConstants(1, 16, &transProjMatrix, 16);
}

void Camera::UpdateLocalAxis()
{
	// ���� z��
	m_n = Vector3::Normalize(m_at);

	// ���� x��
	m_u = Vector3::Normalize(Vector3::Cross(m_up, m_n));

	// ���� y��
	m_v = Vector3::Cross(m_n, m_u);
}

void Camera::Move(const XMFLOAT3& shift)
{
	m_eye = Vector3::Add(m_eye, shift);
}

void Camera::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	if (roll != 0.0f)
	{
		// roll�� ��� -89 ~ +89��
		XMMATRIX rotate{ XMMatrixIdentity() };
		if (m_roll + roll > MAX_ROLL)
		{
			rotate = XMMatrixRotationAxis(XMLoadFloat3(&m_u), XMConvertToRadians(MAX_ROLL - m_roll));
			m_roll = MAX_ROLL;
		}
		else if (m_roll + roll < MIN_ROLL)
		{
			rotate = XMMatrixRotationAxis(XMLoadFloat3(&m_u), XMConvertToRadians(MIN_ROLL - m_roll));
			m_roll = MIN_ROLL;
		}
		else
		{
			rotate = XMMatrixRotationAxis(XMLoadFloat3(&m_u), XMConvertToRadians(roll));
			m_roll += roll;
		}
		XMStoreFloat3(&m_at, XMVector3TransformNormal(XMLoadFloat3(&m_at), rotate));
	}
	if (pitch != 0.0f)
	{
		// pitch�� ��� ���� ����
		m_pitch += pitch;

		XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_v), XMConvertToRadians(pitch)) };
		XMStoreFloat3(&m_at, XMVector3TransformNormal(XMLoadFloat3(&m_at), rotate));
	}
	if (yaw != 0.0f)
	{
		// z�����δ� ȸ���� �� ����
	}
	UpdateLocalAxis();
}

void Camera::SetPlayer(const shared_ptr<Player>& player)
{
	if (m_player) m_player.reset();
	m_player = player;
}