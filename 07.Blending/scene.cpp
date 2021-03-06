#include "scene.h"

void ResourceManager::ReleaseUploadBuffer() const
{
	for (auto [_, mesh] : m_meshes)
		mesh->ReleaseUploadBuffer();
	for (auto [_, texture] : m_textures)
		texture->ReleaseUploadBuffer();
}

shared_ptr<Mesh> ResourceManager::GetMesh(const string& key) const
{
	auto value{ m_meshes.find(key) };
	if (value == m_meshes.end())
		return nullptr;
	return value->second;
}

shared_ptr<Shader> ResourceManager::GetShader(const string& key) const
{
	auto value{ m_shaders.find(key) };
	if (value == m_shaders.end())
		return nullptr;
	return value->second;
}

shared_ptr<Texture> ResourceManager::GetTexture(const string& key) const
{
	auto value{ m_textures.find(key) };
	if (value == m_textures.end())
		return nullptr;
	return value->second;
}

// --------------------------------------

void Scene::OnInit(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature, FLOAT aspectRatio)
{
	// 府家胶概聪历 积己
	m_resourceManager = make_unique<ResourceManager>();

	// 皋浆 积己
	auto cubeMesh{ make_shared<CubeMesh>(device, commandList, 0.5f, 0.5f, 0.5f) };
	auto bulletMesh{ make_shared<CubeMesh>(device, commandList, 0.1f, 0.1f, 0.1f) };
	auto explosionMesh{ make_shared<TextureRectMesh>(device, commandList, 5.0f, 0.0f, 5.0f, XMFLOAT3{}) };

	// 嘉捞歹 积己
	auto shader{ make_shared<Shader>(device, rootSignature) };
	auto terrainShader{ make_shared<TerrainShader>(device, rootSignature) };
	auto terrainTessShader{ make_shared<TerrainTessShader>(device, rootSignature) };
	auto terrainTessWireShader{ make_shared<TerrainTessWireShader>(device, rootSignature) };
	auto blendingShader{ make_shared<BlendingShader>(device, rootSignature) };

	// 咆胶媚 积己
	auto rockTexture{ make_shared<Texture>() };
	rockTexture->LoadTextureFile(device, commandList, 2, PATH("Rock.dds"));
	rockTexture->CreateSrvDescriptorHeap(device);
	rockTexture->CreateShaderResourceView(device);

	auto terrainTexture{ make_shared<Texture>() };
	terrainTexture->LoadTextureFile(device, commandList, 2, PATH("BaseTerrain.dds"));
	terrainTexture->LoadTextureFile(device, commandList, 3, PATH("DetailTerrain.dds"));
	terrainTexture->CreateSrvDescriptorHeap(device);
	terrainTexture->CreateShaderResourceView(device);

	auto explosionTexture{ make_shared<Texture>() };
	for (int i = 1; i <= 33; ++i)
		explosionTexture->LoadTextureFile(device, commandList, 2, PATH("explosion (" + to_string(i) + ").dds"));
	explosionTexture->CreateSrvDescriptorHeap(device);
	explosionTexture->CreateShaderResourceView(device);

	// 府家胶概聪历俊 府家胶 眠啊
	m_resourceManager->AddMesh("CUBE", cubeMesh);
	m_resourceManager->AddMesh("BULLET", bulletMesh);
	m_resourceManager->AddMesh("EXPLOSION", explosionMesh);
	m_resourceManager->AddShader("TEXTURE", shader);
	m_resourceManager->AddShader("TERRAIN", terrainShader);
	m_resourceManager->AddShader("TERRAINTESSWIRE", terrainTessWireShader);
	m_resourceManager->AddShader("TERRAINTESS", terrainTessShader);
	m_resourceManager->AddShader("BLENDING", blendingShader);
	m_resourceManager->AddTexture("ROCK", rockTexture);
	m_resourceManager->AddTexture("TERRAIN", terrainTexture);
	m_resourceManager->AddTexture("EXPLOSION", explosionTexture);

	// 墨皋扼 积己
	auto camera{ make_shared<ThirdPersonCamera>() };
	SetCamera(camera);

	// 墨皋扼 捧康 青纺 汲沥
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio, 1.0f, 5000.0f));
	camera->SetProjMatrix(projMatrix);

	// 敲饭捞绢 积己
	auto player{ make_shared<Player>() };
	player->SetMesh(m_resourceManager->GetMesh("CUBE"));
	player->SetShader(m_resourceManager->GetShader("TEXTURE"));
	player->SetTexture(m_resourceManager->GetTexture("ROCK"));
	SetPlayer(player);

	// 墨皋扼, 敲饭捞绢 辑肺 汲沥
	camera->SetPlayer(player);
	player->SetCamera(camera);

	// 胶墨捞冠胶 积己
	auto skybox{ make_unique<Skybox>(device, commandList, rootSignature) };
	skybox->SetCamera(camera);
	SetSkybox(skybox);

	// 瘤屈 积己
	// 茄 喉废 寸 25俺狼 沥痢栏肺 捞风绢廉乐栏骨肺 喉废 呈厚, 辨捞绰 4狼 硅荐咯具茄促.
	XMFLOAT3 terrainScale{ 1.0f, 0.2f, 1.0f };
	auto terrain{ make_unique<HeightMapTerrain>(device, commandList, PATH("HeightMap.raw"), m_resourceManager->GetShader("TERRAINTESS"), 
												m_resourceManager->GetTexture("TERRAIN"), 257, 257, 12, 12, terrainScale) };
	terrain->SetPosition({ 0.0f, 0.0f, 0.0f });
	m_terrains.push_back(move(terrain));
}

void Scene::OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime)
{
	// 拳搁 啊款单 谅钎 拌魂
	RECT rect; GetWindowRect(hWnd, &rect);
	POINT oldMousePosition{ rect.left + width / 2, rect.top + height / 2 };

	// 框流牢 付快胶 谅钎
	POINT newMousePosition; GetCursorPos(&newMousePosition);

	// 框流牢 沥档俊 厚肥秦辑 雀傈
	int dx = newMousePosition.x - oldMousePosition.x;
	int dy = newMousePosition.y - oldMousePosition.y;
	float sensitive{ 2.5f };
	if (m_player) m_player->Rotate(0.0f, dy * sensitive * deltaTime, dx * sensitive * deltaTime);

	// 付快胶甫 拳搁 啊款单肺 捞悼
	SetCursorPos(oldMousePosition.x, oldMousePosition.y);
}

void Scene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CreateBullet();
}

void Scene::OnKeyboardEvent(FLOAT deltaTime)
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

void Scene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 瘤屈 客捞绢橇饭烙 ON, OFF
	// 
	static bool drawAsWireframe{ false };
	if (wParam == 'q' || wParam == 'Q')
	{
		drawAsWireframe = !drawAsWireframe;
		for (auto& terrain : m_terrains)
			if (drawAsWireframe)
				terrain->SetShader(m_resourceManager->GetShader("TERRAINTESSWIRE"));
			else
				terrain->SetShader(m_resourceManager->GetShader("TERRAINTESS"));
	}

	// 辆丰
	else if (wParam == VK_ESCAPE)
	{
		exit(0);
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
	for (auto& particle : m_particles)
		particle->Update(deltaTime);
}

void Scene::Update(FLOAT deltaTime)
{
	RemoveDeletedObjects();
	UpdateObjectsTerrain();
}

void Scene::RemoveDeletedObjects()
{
	vector<unique_ptr<GameObject>> willBeAdded;

	auto pred = [&](unique_ptr<GameObject>& object) {
		if (object->isDeleted() && object->GetType() == GameObjectType::BULLET)
		{
			auto textureInfo{ make_unique<TextureInfo>() };
			textureInfo->frameInterver *= 1.5f;
			textureInfo->isFrameRepeat = false;

			auto explosion{ make_unique<BillboardObject>(m_camera) };
			explosion->SetPosition(object->GetPosition());
			explosion->SetMesh(m_resourceManager->GetMesh("EXPLOSION"));
			explosion->SetShader(m_resourceManager->GetShader("BLENDING"));
			explosion->SetTexture(m_resourceManager->GetTexture("EXPLOSION"));
			explosion->SetTextureInfo(textureInfo);
			willBeAdded.push_back(move(explosion));
		}
		return object->isDeleted();
	};
	m_gameObjects.erase(remove_if(m_gameObjects.begin(), m_gameObjects.end(), pred), m_gameObjects.end());
	m_particles.erase(remove_if(m_particles.begin(), m_particles.end(), pred), m_particles.end());

	// 醚舅 昏力瞪 锭 积扁绰 捞棋飘绰 颇萍努 按眉俊 眠啊茄促.
	for (auto& object : willBeAdded)
		m_particles.push_back(move(object));
}

void Scene::UpdateObjectsTerrain()
{
	XMFLOAT3 pos{};
	auto pred = [&pos](unique_ptr<HeightMapTerrain>& terrain) {
		XMFLOAT3 tPos{ terrain->GetPosition() };
		XMFLOAT3 scale{ terrain->GetScale() };
		float width{ terrain->GetWidth() * scale.x };
		float length{ terrain->GetLength() * scale.z };

		// 窍疵俊辑 +z绵阑 赣府率栏肺 滴绊 瘤屈阑 好阑 锭甫 扁霖
		float left{ tPos.x };
		float right{ tPos.x + width };
		float top{ tPos.z + length };
		float bot{ tPos.z };

		if ((left <= pos.x && pos.x <= right) &&
			(bot <= pos.z && pos.z <= top))
			return true;
		return false;
	};

	if (m_player)
	{
		pos = m_player->GetPosition();
		auto terrain = find_if(m_terrains.begin(), m_terrains.end(), pred);
		m_player->SetTerrain(terrain != m_terrains.end() ? terrain->get() : nullptr);
	}
	for (auto& object : m_particles)
	{
		pos = object->GetPosition();
		auto terrain = find_if(m_terrains.begin(), m_terrains.end(), pred);
		object->SetTerrain(terrain != m_terrains.end() ? terrain->get() : nullptr);
	}
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// 墨皋扼 嘉捞歹 函荐(轰, 捧康 函券 青纺) 弥脚拳
	if (m_camera) m_camera->UpdateShaderVariable(commandList);

	// 胶墨捞冠胶 坊歹傅, 啊厘 刚历 坊歹傅秦具窃
	// 胶墨捞冠胶绰 表捞滚欺俊 康氢阑 固摹瘤 臼绰促(SkyboxShader).
	if (m_skybox) m_skybox->Render(commandList);

	// 敲饭捞绢 坊歹傅
	if (m_player) m_player->Render(commandList);

	// 霸烙坷宏璃飘 坊歹傅
	for (const auto& gameObject : m_gameObjects)
		gameObject->Render(commandList);

	// 瘤屈 坊歹傅
	for (const auto& terrain : m_terrains)
		terrain->Render(commandList);

	// 颇萍努 坊歹傅
	for (const auto& particle : m_particles)
		particle->Render(commandList);
}

void Scene::ReleaseUploadBuffer()
{
	if (m_resourceManager) m_resourceManager->ReleaseUploadBuffer();
}

void Scene::CreateBullet()
{
	unique_ptr<Bullet> bullet{ make_unique<Bullet>(m_player->GetPosition(), m_player->GetLook(), m_player->GetNormal(), 100.0f) };
	bullet->SetPosition(m_player->GetPosition());
	bullet->SetMesh(m_resourceManager->GetMesh("BULLET"));
	bullet->SetShader(m_resourceManager->GetShader("BLENDING"));
	bullet->SetTexture(m_resourceManager->GetTexture("ROCK"));
	//m_gameObjects.push_back(move(bullet));
	m_particles.push_back(move(bullet));
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

		// 瘤屈阑 窍疵俊辑 关栏肺 好阑 锭
		float left{ pos.x };			// 哭率
		float right{ pos.x + width };	// 坷弗率
		float top{ pos.z + length };	// 困
		float bot{ pos.z };				// 关

		if ((left <= x && x <= right) && (bot <= z && z <= top))
			return true;
		return false;
		});

	return terrain != m_terrains.end() ? terrain->get() : nullptr;
}