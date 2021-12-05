#include "shadow.h"

ShadowMap::ShadowMap(const ComPtr<ID3D12Device>& device, UINT width, UINT height) : m_width{ width }, m_height{ height }, m_format{ DXGI_FORMAT_R24G8_TYPELESS }
{
	m_viewport = { 0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f };
	m_scissorRect = { 0, 0, static_cast<int>(m_width), static_cast<int>(m_height) };

	// 렌더타겟뷰 서술자힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = 1;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

	// 렌더타겟뷰 서술자 생성
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;

	// 텍스쳐 생성
	ComPtr<ID3D12Resource> shadowMap;
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(m_format, m_width, m_height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 },
		IID_PPV_ARGS(&shadowMap)
	));

	// 렌더타겟뷰 생성
	device->CreateRenderTargetView(shadowMap.Get(), &rtvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart() });
}