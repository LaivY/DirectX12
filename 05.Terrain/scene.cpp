#include "scene.h"

void Scene::OnInit(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature, FLOAT aspectRatio)
{
	// ���⼭ �����ϴ� ��ü���� ����� �����ϱ� ������ ����� �Ϸ�Ǳ� ������ �޸� ���� �����ؾ���
	// -> ����Ʈ�����ͷ� ��� ������Ʈ������ ������� �ʴ� �޽�, �ؽ��ĸ� ����� �� �Լ��� ����� �����ǹǷ� ������ �߻���

	// ī�޶� ����
	shared_ptr<ThirdPersonCamera> camera{ make_shared<ThirdPersonCamera>() };
	camera->SetEye(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	camera->SetAt(XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	camera->SetUp(XMFLOAT3{ 0.0f, 1.0f, 0.0f });

	// ī�޶� ���� ��� ����
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio, 0.1f, 3000.0f));
	camera->SetProjMatrix(projMatrix);

	// �޽� ����
	shared_ptr<CubeMesh> mesh{ make_shared<CubeMesh>(device, commandList, 0.5f, 0.5f, 0.5f) };

	// �ؽ��� ����
	shared_ptr<Texture> rockTexture{ make_shared<Texture>() };
	rockTexture->LoadTextureFile(device, commandList, TEXT("resource/rock.dds"), 2);
	rockTexture->CreateSrvDescriptorHeap(device);
	rockTexture->CreateShaderResourceView(device);

	// �⺻ ���̴� ����
	shared_ptr<Shader> shader{ make_shared<Shader>(device, rootSignature) };

	// ������ ��ü ����
	//shared_ptr<TextureRectMesh> textureRectMesh{ make_shared<TextureRectMesh>(device, commandList, 10.0f, 0.0f, 10.0f, XMFLOAT3{}) };
	//unique_ptr<BillboardObject> obj{ make_unique<BillboardObject>(camera) };
	//obj->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 5.0f });
	//obj->SetMesh(textureRectMesh);
	//obj->SetShader(shader);
	//obj->SetTexture(rockTexture);
	//m_gameObjects.push_back(move(obj));

	// �÷��̾� ����
	shared_ptr<Player> player{ make_shared<Player>() };
	player->SetMesh(mesh);
	player->SetShader(shader);
	player->SetTexture(rockTexture);

	// ��, ī�޶� �÷��̾� ����
	SetPlayer(player);
	camera->SetPlayer(m_player);

	// ��, �÷��̾� ī�޶� ����
	SetCamera(camera);
	player->SetCamera(camera);

	// ���� ����
	shared_ptr<TerrainShader> terrainShader{ make_shared<TerrainShader>(device, rootSignature) };
	shared_ptr<Texture> terrainTexture{ make_shared<Texture>() };
	terrainTexture->LoadTextureFile(device, commandList, TEXT("resource/BaseTerrain.dds"), 2); // BaseTexture
	terrainTexture->LoadTextureFile(device, commandList, TEXT("resource/DetailTerrain.dds"), 3); // DetailTexture
	terrainTexture->CreateSrvDescriptorHeap(device);
	terrainTexture->CreateShaderResourceView(device);

	unique_ptr<HeightMapTerrain> terrain{
		make_unique<HeightMapTerrain>(device, commandList, TEXT("resource/heightMap.raw"), terrainShader, terrainTexture, 257, 257, 257, 257, XMFLOAT3{ 1.0f, 0.2f, 1.0f })
	};
	terrain->SetPosition(XMFLOAT3{ 0.0f, -300.0f, 0.0f });
	m_terrains.push_back(move(terrain));

	// ��ī�̹ڽ� ����
	unique_ptr<Skybox> skybox{ make_unique<Skybox>(device, commandList, rootSignature) };
	skybox->SetCamera(camera);
	SetSkybox(skybox);
}

void Scene::OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const
{
	// ȭ�� ��� ��ǥ ���
	RECT rect; GetWindowRect(hWnd, &rect);
	POINT oldMousePosition{ rect.left + width / 2, rect.top + height / 2 };

	// ������ ���콺 ��ǥ
	POINT newMousePosition; GetCursorPos(&newMousePosition);

	// ������ ������ ����ؼ� ȸ��
	int dx = newMousePosition.x - oldMousePosition.x;
	int dy = newMousePosition.y - oldMousePosition.y;
	float sensitive{ 5.0f };
	if (m_player) m_player->Rotate(0.0f, dy * sensitive * deltaTime, dx * sensitive * deltaTime);

	// ���콺�� ȭ�� ����� �̵�
	SetCursorPos(oldMousePosition.x, oldMousePosition.y);
}

void Scene::OnKeyboardEvent(FLOAT deltaTime) const
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetFront(), deltaTime * 10.0f));
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetRight(), deltaTime * -10.0f));
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetFront(), deltaTime * -10.0f));
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetRight(), deltaTime * 10.0f));
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetUp(), deltaTime * 10.0f));
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetUp(), deltaTime * -10.0f));
	}
}

void Scene::OnUpdate(FLOAT deltaTime)
{
	Update(deltaTime);
	if (m_player) m_player->Update(deltaTime);
	if (m_camera) m_camera->Update(deltaTime);
	if (m_skybox) m_skybox->Update();
	for (auto& object : m_gameObjects)
		object->Update(deltaTime);
}

void Scene::Update(FLOAT deltaTime)
{
	// �÷��̾ � ���� ���� �ִ��� ��������
	if (m_player)
	{
		XMFLOAT3 playerPos{ m_player->GetPosition() };
		auto terrain = find_if(m_terrains.begin(), m_terrains.end(), [&playerPos](unique_ptr<HeightMapTerrain>& terrain) {
			XMFLOAT3 pos{ terrain->GetPosition() };
			XMFLOAT3 scale{ terrain->GetScale() };
			float width{ terrain->GetWidth() * scale.x };
			float length{ terrain->GetLength() * scale.z };

			// �ϴÿ��� ������ ���� ���� ����
			float left{ pos.x };
			float right{ pos.x + width };
			float top{ pos.z + length };
			float bot{ pos.z };

			if ((left <= playerPos.x && playerPos.x <= right) &&
				(bot <= playerPos.z && playerPos.z <= top))
				return true;

			return false;
			});

		m_player->SetTerrain(terrain != m_terrains.end() ? terrain->get() : nullptr);
	}
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// ī�޶� ���̴� ����(��, ���� ��ȯ ���) �ֽ�ȭ
	if (m_camera) m_camera->UpdateShaderVariable(commandList);

	// ��ī�̹ڽ� ������, ���� ���� �������ؾ���
	// ��ī�̹ڽ��� ���̹��ۿ� ������ ��ġ�� �ʴ´�(SkyboxShader).
	if (m_skybox) m_skybox->Render(commandList);

	// �÷��̾� ������
	if (m_player) m_player->Render(commandList);

	// ���ӿ�����Ʈ ������
	for (const auto& gameObject : m_gameObjects)
		gameObject->Render(commandList);

	// ���� ������
	for (const auto& terrain : m_terrains)
		terrain->Render(commandList);
}

void Scene::ReleaseUploadBuffer()
{
	for (const auto& gameObject : m_gameObjects)
		gameObject->ReleaseUploadBuffer();
}

void Scene::SetSkybox(unique_ptr<Skybox>& skybox)
{
	if (m_skybox) m_skybox.reset();
	m_skybox = move(skybox);
}

void Scene::SetPlayer(const shared_ptr<Player>& player)
{
	if (m_player) m_player.reset();
	m_player = player;
}

void Scene::SetCamera(const shared_ptr<Camera>& camera)
{
	if (m_camera) m_camera.reset();
	m_camera = camera;
}

HeightMapTerrain* Scene::GetTerrain(FLOAT x, FLOAT z) const
{
	auto terrain = find_if(m_terrains.begin(), m_terrains.end(), [&x, &z](const unique_ptr<HeightMapTerrain>& t) {
		XMFLOAT3 scale{ t->GetScale() };
		XMFLOAT3 pos{ t->GetPosition() };
		float width{ t->GetWidth() * scale.x };
		float length{ t->GetLength() * scale.z };

		// ������ �ϴÿ��� ������ ���� ��
		float left{ pos.x };			// ����
		float right{ pos.x + width };	// ������
		float top{ pos.z + length };	// ��
		float bot{ pos.z };				// ��

		if ((left <= x && x <= right) && (bot <= z && z <= top))
			return true;
		return false;
		});

	return terrain != m_terrains.end() ? terrain->get() : nullptr;
}