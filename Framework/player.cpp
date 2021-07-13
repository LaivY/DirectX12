#include "player.h"
#include "camera.h"

Player::Player() : GameObject{}, m_velocity{ 0.0f, 0.0f, 0.0f }, m_maxVelocity{ 0.01f, 0.01f, 0.01f }, m_friction{ 0.9f }
{

}

void Player::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	GameObject::Rotate(roll, pitch, yaw);
	m_camera->Rotate(roll, pitch, yaw);
}

void Player::AddVelocity(const XMFLOAT3& increase)
{
	m_velocity = Vector3::Add(m_velocity, increase);

	// X, Y, Z 최대 속도를 넘지못하게 설정
	m_velocity.x = clamp(m_velocity.x, -m_maxVelocity.x, m_maxVelocity.x);
	m_velocity.y = clamp(m_velocity.y, -m_maxVelocity.y, m_maxVelocity.y);
	m_velocity.z = clamp(m_velocity.z, -m_maxVelocity.z, m_maxVelocity.z);
}