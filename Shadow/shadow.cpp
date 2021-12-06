#include "shadow.h"

ShadowMap::ShadowMap(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, UINT width, UINT height) 
	: m_width{ width }, m_height{ height }, m_format{ DXGI_FORMAT_R24G8_TYPELESS },
	  m_viewport{ 0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f },
	  m_scissorRect{ 0, 0, static_cast<int>(m_width), static_cast<int>(m_height) }
{
	CreateSrvDsvDescriptorHeap(device);
	CreateShaderResourceView(device);
	CreateDepthStencilView(device);
	CreateCamera(device, commandList);
}

void ShadowMap::CreateSrvDsvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	// 셰이더리소스뷰 서술자힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

	// 깊이스텐실 서술자힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
}

void ShadowMap::CreateShaderResourceView(const ComPtr<ID3D12Device>& device)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	device->CreateShaderResourceView(m_shadowMap.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
}

void ShadowMap::CreateDepthStencilView(const ComPtr<ID3D12Device>& device)
{
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(m_format, m_width, m_height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 },
		IID_PPV_ARGS(&m_shadowMap)
	));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	device->CreateDepthStencilView(m_shadowMap.Get(), &depthStencilViewDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void ShadowMap::CreateCamera(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	/*
	* 방향성 조명으로 만들어진 그림자를 구현할 것이다.
	* 따라서 카메라의 투영행렬은 직교투영행렬을 사용한다.
	* 이 클래스에서 카메라를 가지는 건 이상하므로 일단 코드를 작성하고 추후에 옮긴다.
	*/

	m_camera = make_unique<Camera>();

	XMFLOAT3 lightPos{ -100.0f, 100.0f, 0.0f };
	XMFLOAT3 lightDir{ 1.0f, -1.0f, 0.0f };
	XMFLOAT3 lightUp{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3 targetPos{ 0.0f, 0.0f, 0.0f };

	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&targetPos), XMLoadFloat3(&lightUp)));
	m_camera->SetViewMatrix(viewMatrix);

	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixOrthographicLH(m_width, m_height, 1.0f, 100.0f));
	m_camera->SetProjMatrix(projMatrix);
}