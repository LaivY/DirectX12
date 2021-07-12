#pragma once
#include "stdafx.h"
#include "object.h"
#include "camera.h"
#include "timer.h"

class GameFramework
{
public:
	GameFramework(UINT width, UINT height);
	~GameFramework();

	void GameLoop();
	void OnInit(HINSTANCE hInstance, HWND hWnd);
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	void OnMouseEvent();
	void OnKeyboardEvent();

	void CreateDevice(const ComPtr<IDXGIFactory4>& factory);
	void CreateCommandQueue();
	void CreateSwapChain(const ComPtr<IDXGIFactory4>& factory);
	void CreateRtvDsvDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateRootSignature();
	void CreatePipelineStateObject();

	void LoadPipeline();
	void LoadAssets();

	void PopulateCommandList();
	void WaitForPreviousFrame();

	UINT GetWindowWidth() const { return m_width; }
	UINT GetWindowHeight() const { return m_height; }
	void SetIsActive(BOOL isActive) { m_isActive = isActive; }

private:
	static const UINT					FrameCount = 2;

	// Window
	HINSTANCE							m_hInstance;
	HWND								m_hWnd;
	UINT								m_width;
	UINT								m_height;
	FLOAT								m_aspectRatio;
	POINT								m_mousePosition;
	BOOL								m_isActive;

	// Pipeline
	D3D12_VIEWPORT						m_viewport;
	D3D12_RECT							m_scissorRect;
	ComPtr<IDXGISwapChain3>				m_swapChain;
	INT									m_MSAA4xQualityLevel;
	ComPtr<ID3D12Device>				m_device;
	ComPtr<ID3D12CommandAllocator>		m_commandAllocator;
	ComPtr<ID3D12CommandQueue>			m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList>	m_commandList;
	ComPtr<ID3D12Resource>				m_renderTargets[FrameCount];
	ComPtr<ID3D12DescriptorHeap>		m_rtvHeap;
	UINT								m_rtvDescriptorSize;
	ComPtr<ID3D12Resource>				m_depthStencil;
	ComPtr<ID3D12DescriptorHeap>		m_dsvHeap;
	ComPtr<ID3D12RootSignature>			m_rootSignature;
	ComPtr<ID3D12PipelineState>			m_pipelineState;

	// Synchronization
	ComPtr<ID3D12Fence>					m_fence;
	UINT								m_frameIndex;
	UINT64								m_fenceValue;
	HANDLE								m_fenceEvent;

	// Timer
	Timer								m_timer;

	// GameObjects
	vector<unique_ptr<GameObject>>		m_gameObjects;
	shared_ptr<Player>					m_player;
	shared_ptr<Camera>					m_camera;
};