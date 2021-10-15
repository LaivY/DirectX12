#include "player.h"
#include "camera.h"

Player::Player() : GameObject{}, m_velocity{ 0.0f, 0.0f, 0.0f }, m_maxVelocity{ 10.0f }, m_friction{ 1.1f }, m_terrain{ nullptr }
{

}

void Player::Update(FLOAT deltaTime)
{
	// �̵�
	Move(m_velocity);

	// �÷��̾ � ���� ���� �ִٸ�
	if (m_terrain)
	{
		// �÷��̾ ���� ���� �̵��ϵ���
		XMFLOAT3 pos{ GetPosition() };
		FLOAT height{ m_terrain->GetHeight(pos.x, pos.z) };
		SetPosition(XMFLOAT3{ pos.x, height + 0.5f, pos.z });

		// �÷��̾ ���ִ� ���� �븻�� ����
		XMFLOAT3 normal{ m_terrain->GetNormal(pos.x, pos.z) };
		m_normal = normal;
	}

	// ������ ����
	m_velocity = Vector3::Mul(m_velocity, 1 / m_friction * deltaTime);
}

void Player::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// ȸ���� ����
	if (m_pitch + pitch > MAX_PITCH)
		pitch = MAX_PITCH - m_pitch;
	else if (m_pitch + pitch < MIN_PITCH)
		pitch = MIN_PITCH - m_pitch;

	// ȸ���� �ջ�
	m_roll += roll; m_pitch += pitch; m_yaw += yaw;

	// ī�޶�� x,y������ ȸ���� �� �ִ�.
	// GameObject::Rotate���� �÷��̾��� ���� x,y,z���� �����ϹǷ� ���� ȣ���ؾ��Ѵ�.
	m_camera->Rotate(0.0f, pitch, yaw);

	// �÷��̾�� y�����θ� ȸ���� �� �ִ�.
	GameObject::Rotate(0.0f, 0.0f, yaw);
}

void Player::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	XMFLOAT4X4 worldMatrix{ m_worldMatrix };
	
	float theta{ acosf(Vector3::Dot(m_up, m_normal)) };
	if (XMConvertToDegrees(theta))
	{
		// +z���� �������� ���� right����
		XMFLOAT3 right{ Vector3::Cross(m_up, m_normal) };

		// +z��� m_front�� ���̰�
		//float _theta{ acosf(Vector3::Dot(XMFLOAT3{ 0.0f, 0.0f, 1.0f }, m_front)) };
		//if (m_front.x < 0) _theta *= -1;

		//XMFLOAT4X4 _rotate;
		//XMStoreFloat4x4(&_rotate, XMMatrixRotationAxis(XMLoadFloat3(&m_normal), _theta));
		//right = Vector3::TransformNormal(right, _rotate);

		XMFLOAT4X4 rotate;
		XMStoreFloat4x4(&rotate, XMMatrixRotationAxis(XMLoadFloat3(&right), theta));
		worldMatrix = Matrix::Mul(rotate, worldMatrix);
	}

	commandList->SetGraphicsRoot32BitConstants(0, 16, &Matrix::Transpose(worldMatrix), 0);
}

void Player::AddVelocity(const XMFLOAT3& increase)
{
	m_velocity = Vector3::Add(m_velocity, increase);

	// �ִ� �ӵ��� �ɸ��ٸ� �ش� ������ ��ҽ�Ŵ
	FLOAT length{ Vector3::Length(m_velocity) };
	if (length > m_maxVelocity)
	{
		FLOAT ratio{ m_maxVelocity / length };
		m_velocity = Vector3::Mul(m_velocity, ratio);
	}
}