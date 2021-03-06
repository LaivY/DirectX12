#include "player.h"
#include "camera.h"

Player::Player() : GameObject{}, m_velocity{ 0.0f, 0.0f, 0.0f }, m_maxVelocity{ 10.0f }, m_friction{ 1.1f }
{

}

void Player::Update(FLOAT deltaTime)
{
	Move(m_velocity);

	m_look = m_front;

	// 플레이어가 어떤 지형 위에 있다면
	if (m_terrain)
	{
		// 플레이어가 지형 위에 있도록 설정
		SetPlayerOnTerrain();

		// 플레이어가 서있는 곳의 노말 설정
		XMFLOAT3 pos{ GetPosition() };
		m_normal = m_terrain->GetNormal(pos.x, pos.z);

		// 플레이어가 바라보는 방향 설정
		if (float theta = acosf(Vector3::Dot(m_up, m_normal)))
		{
			XMFLOAT3 right{ Vector3::Normalize(Vector3::Cross(m_up, m_normal)) };
			XMFLOAT4X4 rotate; XMStoreFloat4x4(&rotate, XMMatrixRotationNormal(XMLoadFloat3(&right), theta));
			m_look = Vector3::TransformNormal(GetFront(), rotate);
		}
	}

	m_velocity = Vector3::Mul(m_velocity, 1 / m_friction * deltaTime);
}

void Player::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// 회전각 제한
	if (m_pitch + pitch > MAX_PITCH)
		pitch = MAX_PITCH - m_pitch;
	else if (m_pitch + pitch < MIN_PITCH)
		pitch = MIN_PITCH - m_pitch;

	// 회전각 합산
	m_roll += roll; m_pitch += pitch; m_yaw += yaw;

	// 카메라는 x,y축으로 회전할 수 있다.
	// GameObject::Rotate에서 플레이어의 로컬 x,y,z축을 변경하므로 먼저 호출해야한다.
	m_camera->Rotate(0.0f, pitch, yaw);

	// 플레이어는 y축으로만 회전할 수 있다.
	GameObject::Rotate(0.0f, 0.0f, yaw);
}

void Player::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	XMFLOAT4X4 worldMatrix{ m_worldMatrix };
	XMFLOAT3 up{ m_normal };
	XMFLOAT3 look{ m_look };
	XMFLOAT3 right{ Vector3::Normalize(Vector3::Cross(up, look)) };
	worldMatrix._11 = right.x;	worldMatrix._12 = right.y;	worldMatrix._13 = right.z;
	worldMatrix._21 = up.x;		worldMatrix._22 = up.y;		worldMatrix._23 = up.z;
	worldMatrix._31 = look.x;	worldMatrix._32 = look.y;	worldMatrix._33 = look.z;
	commandList->SetGraphicsRoot32BitConstants(0, 16, &Matrix::Transpose(worldMatrix), 0);
}

void Player::SetPlayerOnTerrain()
{
	/*
	1. 플레이어가 위치한 블록의 좌측하단 좌표를 구한다.
	2. 해당 좌표를 이용하여 블록 메쉬의 정점 25개를 구한다.
	3. 정점 25개를 이용하여 4차 베지어 곡면을 구한다.
	4. 플레이어의 위치 t를 구해서 베지어 곡면 상의 높이를 구한다.
	*/

	XMFLOAT3 pos{ GetPosition() };
	XMFLOAT3 LB{ m_terrain->GetBlockPosition(pos.x, pos.z) };

	int width{ m_terrain->GetWidth() };
	int length{ m_terrain->GetLength() };
	int blockWidth{ m_terrain->GetBlockWidth() };
	int blockLength{ m_terrain->GetBlockLength() };
	XMFLOAT3 scale{ m_terrain->GetScale() };

	auto CubicBezierSum = [](const array<XMFLOAT3, 25>& patch, XMFLOAT2 t) {

		// 4차 베지어 곡선 계수
		array<float, 5> uB, vB;
		float txInv{ 1.0f - t.x };
		uB[0] = txInv * txInv * txInv * txInv;
		uB[1] = 4.0f * t.x * txInv * txInv * txInv;
		uB[2] = 6.0f * t.x * t.x * txInv * txInv;
		uB[3] = 4.0f * t.x * t.x * t.x * txInv;
		uB[4] = t.x * t.x * t.x * t.x;

		float tyInv{ 1.0f - t.y };
		vB[0] = tyInv * tyInv * tyInv * tyInv;
		vB[1] = 4.0f * t.y * tyInv * tyInv * tyInv;
		vB[2] = 6.0f * t.y * t.y * tyInv * tyInv;
		vB[3] = 4.0f * t.y * t.y * t.y * tyInv;
		vB[4] = t.y * t.y * t.y * t.y;

		// 4차 베지에 곡면 계산
		XMFLOAT3 sum{ 0.0f, 0.0f, 0.0f };
		for (int i = 0; i < 5; ++i)
		{
			XMFLOAT3 subSum{ 0.0f, 0.0f, 0.0f };
			for (int j = 0; j < 5; ++j)
			{
				XMFLOAT3 temp{ Vector3::Mul(patch[(i * 5) + j], uB[j]) };
				subSum = Vector3::Add(subSum, temp);
			}
			subSum = Vector3::Mul(subSum, vB[i]);
			sum = Vector3::Add(sum, subSum);
		}
		return sum;
	};

	// 베지에 평면 제어점 25개
	array<XMFLOAT3, 25> vertices;
	for (int i = 0, z = 4; z >= 0; --z)
	{
		for (int x = 0; x < 5; ++x)
		{
			vertices[i].x = (LB.x + x * blockWidth / 4) * scale.x;
			vertices[i].z = (LB.z + z * blockLength / 4) * scale.z;
			vertices[i].y = m_terrain->GetHeight(vertices[i].x, vertices[i].z);
			++i;
		}
	}

	// 플레이어의 위치 t
	XMFLOAT2 uv{ (pos.x - LB.x) / blockWidth, 1.0f - (pos.z - LB.z) / blockLength };
	XMFLOAT3 temp{ CubicBezierSum(vertices, uv) };

	// 플레이어를 노말 방향으로 0.5만큼 움직임
	temp = Vector3::Add(temp, Vector3::Mul(m_normal, 0.5f));
	SetPosition(XMFLOAT3{ pos.x, temp.y, pos.z });
}

void Player::AddVelocity(const XMFLOAT3& increase)
{
	m_velocity = Vector3::Add(m_velocity, increase);

	// 최대 속도에 걸린다면 해당 비율로 축소시킴
	FLOAT length{ Vector3::Length(m_velocity) };
	if (length > m_maxVelocity)
	{
		FLOAT ratio{ m_maxVelocity / length };
		m_velocity = Vector3::Mul(m_velocity, ratio);
	}
}