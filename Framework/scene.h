#pragma once
#include "stdafx.h"
#include "object.h"
#include "player.h"
#include "camera.h"

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	vector<unique_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }
	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

private:
	vector<unique_ptr<GameObject>>	m_gameObjects;
	shared_ptr<Player>				m_player;
	shared_ptr<Camera>				m_camera;
};