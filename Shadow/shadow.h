#pragma once
#include "stdafx.h"
#include "camera.h"
#include "shader.h"
#include "object.h"

class ShadowMap
{
public:
	ShadowMap(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, UINT width, UINT height);
	~ShadowMap() = default;

	void CreateSrvDsvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateShaderResourceView(const ComPtr<ID3D12Device>& device);
	void CreateDepthStencilView(const ComPtr<ID3D12Device>& device);
	void CreateCamera(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList);

	D3D12_VIEWPORT GetViewport() const { return m_viewport; }
	D3D12_RECT GetScissorRect() const { return m_scissorRect; }
	ComPtr<ID3D12Resource> GetShadowMap() const { return m_shadowMap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const { return m_dsvHeap->GetCPUDescriptorHandleForHeapStart(); }
	Camera* GetCamera() const { return m_camera.get(); }

private:
	ComPtr<ID3D12Resource>			m_shadowMap;
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	m_dsvHeap;
	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_scissorRect;
	UINT							m_width;
	UINT							m_height;
	DXGI_FORMAT						m_format;
	unique_ptr<Camera>				m_camera;
};