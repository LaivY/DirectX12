#pragma once
#include "stdafx.h"
#include "object.h"
#include "player.h"
#include "camera.h"

class Scene
{
public:
	Scene();
	~Scene();

	void OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnKeyboardEvent(FLOAT deltaTime) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }
	vector<unique_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }

	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

private:
	vector<unique_ptr<GameObject>>	m_gameObjects;
	shared_ptr<Player>				m_player;
	shared_ptr<Camera>				m_camera;
};