#include "texture.h"

void Texture::LoadTextureFile(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName, UINT rootParameterIndex)
{
	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> textureUploadBuffer;

	// DDS 텍스쳐 로딩
	unique_ptr<uint8_t[]> ddsData;
	vector<D3D12_SUBRESOURCE_DATA> subresources;
	DDS_ALPHA_MODE ddsAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
	DX::ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(device.Get(), fileName.c_str(), 0,
		D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, &texture, ddsData, subresources, &ddsAlphaMode));

	// 디폴트 힙으로 데이터 복사하기 위한 업로드 힙 생성
	UINT nSubresources{ (UINT)subresources.size() };
	UINT64 nBytes{ GetRequiredIntermediateSize(texture.Get(), 0, nSubresources) };
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(nBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&textureUploadBuffer)
	));

	// subresources에 있는 데이터를 m_textureBuffer로 복사
	UpdateSubresources(commandList.Get(), texture.Get(), textureUploadBuffer.Get(), 0, 0, nSubresources, subresources.data());

	// 리소스 베리어 설정
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	// 저장
	m_textures.push_back(make_pair(texture, rootParameterIndex));
	m_textureUploadBuffers.push_back(textureUploadBuffer);
}

void Texture::CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device)
{
	if (m_srvHeap) m_srvHeap.Reset();

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = m_textures.size(); // SRV 개수
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void Texture::CreateShaderResourceView(const ComPtr<ID3D12Device>& device)
{
	D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptorHeapStart{ m_srvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (const auto& [texture, _] : m_textures)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = texture->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		device->CreateShaderResourceView(texture.Get(), &srvDesc, srvDescriptorHeapStart);
		srvDescriptorHeapStart.ptr += g_cbvSrvDescriptorIncrementSize;
	}
}

void Texture::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptorHeapStart{ m_srvHeap->GetGPUDescriptorHandleForHeapStart() };
	for (const auto& [_, rootParameterIndex] : m_textures)
	{
		commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvDescriptorHeapStart);
		srvDescriptorHeapStart.ptr += g_cbvSrvDescriptorIncrementSize;
	}
}

void Texture::ReleaseUploadBuffer()
{
	for (auto& textureUploadBuffer : m_textureUploadBuffers)
		textureUploadBuffer.Reset();
}