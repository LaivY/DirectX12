#include "instancing.h"

Instancing::Instancing(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const Mesh& mesh, UINT sizeofData, UINT count)
	: m_instanceBufferView{}, m_instanceBufferPointer{}, m_sizeInBytes{ sizeofData * count }, m_strideInBytes{ sizeofData }
{
	CreatePipelineStateObject(device, rootSignature);
	CreateInstanceBuffer(device);
	m_mesh = make_unique<Mesh>(mesh);
}

Instancing::~Instancing()
{
	m_instanceBuffer->Unmap(0, NULL);
}

void Instancing::CreatePipelineStateObject(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	ComPtr<ID3DBlob> vertexShader, pixelShader;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Instance.hlsl"), NULL, NULL, "VSMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Instance.hlsl"), NULL, NULL, "PSMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));

	// 정점 레이아웃 설정
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
	};

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void Instancing::CreateInstanceBuffer(const ComPtr<ID3D12Device>& device)
{
	// 인스턴스 버퍼 생성
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_instanceBuffer)));

	// 인스턴스 버퍼 포인터
	m_instanceBuffer->Map(0, NULL, reinterpret_cast<void**>(&m_instanceBufferPointer));

	// 인스턴스 버퍼 뷰 생성
	m_instanceBufferView.BufferLocation = m_instanceBuffer->GetGPUVirtualAddress();
	m_instanceBufferView.StrideInBytes = m_strideInBytes;
	m_instanceBufferView.SizeInBytes = m_sizeInBytes;
}

void Instancing::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_mesh)
	{
		commandList->SetPipelineState(m_pipelineState.Get());
		m_mesh->Render(commandList, m_instanceBufferView);
	}
}