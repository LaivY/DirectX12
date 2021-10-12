#pragma once
#include "stdafx.h"
#include "object.h"
#include "player.h"
#include "camera.h"
#include "terrain.h"
#include "skybox.h"

class Scene
{
public:
	Scene();
	~Scene();

	void OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnKeyboardEvent(FLOAT deltaTime) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void AddGameObject(unique_ptr<GameObject> gameObject);
	void AddTerrain(unique_ptr<HeightMapTerrain> terrain);
	void SetSkybox(unique_ptr<Skybox>& skybox);
	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

	Skybox* GetSkybox() const { return m_skybox.get(); }
	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }

private:
	vector<unique_ptr<GameObject>>			m_gameObjects;
	vector<unique_ptr<HeightMapTerrain>>	m_terrain;
	unique_ptr<Skybox>						m_skybox;
	shared_ptr<Player>						m_player;
	shared_ptr<Camera>						m_camera;
};