#pragma once
#include "stdafx.h"

class ShadowMap
{
public:
	ShadowMap(const ComPtr<ID3D12Device>& device, UINT width, UINT height);
	~ShadowMap() = default;

	D3D12_VIEWPORT GetViewport() const { return m_viewport; }
	D3D12_RECT GetScissorRect() const { return m_scissorRect; }
	ComPtr<ID3D12Resource> GetShadowMap() const { return m_shadowMap; }

private:
	ComPtr<ID3D12Resource>			m_shadowMap;
	ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	m_dsvHeap;
	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_scissorRect;
	UINT							m_width;
	UINT							m_height;
	DXGI_FORMAT						m_format;
};