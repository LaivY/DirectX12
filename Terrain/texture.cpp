#include "texture.h"

Texture::Texture(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	// DDS �ؽ��� �ε�
	unique_ptr<uint8_t[]> ddsData;
	vector<D3D12_SUBRESOURCE_DATA> subresources;
	DDS_ALPHA_MODE ddsAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
	DX::ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(device.Get(), fileName.c_str(), 0,
		D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, &m_texture, ddsData, subresources, &ddsAlphaMode));

	// ����Ʈ ������ ������ �����ϱ� ���� ���ε� �� ����
	UINT nSubresources{ (UINT)subresources.size() };
	UINT64 nBytes{ GetRequiredIntermediateSize(m_texture.Get(), 0, nSubresources) };
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(nBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_textureUploadBuffer)
	));

	// subresources�� �ִ� �����͸� m_textureBuffer�� ����
	UpdateSubresources(commandList.Get(), m_texture.Get(), m_textureUploadBuffer.Get(), 0, 0, nSubresources, subresources.data());

	// ���ҽ� ������ ����
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	// ���̴� ���ҽ� �� ������ �� ����
	CreateSrvDescriptorHeap(device);

	// ���̴� ���ҽ� �� ����
	CreateShaderResourceView(device);
}

void Texture::CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 1; // SRV 1��
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void Texture::CreateShaderResourceView(const ComPtr<ID3D12Device>& device)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_texture->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Texture::ReleaseUploadBuffer()
{
	if (m_textureUploadBuffer) m_textureUploadBuffer.Reset();
}