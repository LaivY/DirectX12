#include "framework.h"
#include <fstream>
#include <string>

GameFramework::GameFramework(UINT width, UINT height) :
	m_width{ width },
	m_height{ height },
	m_frameIndex{ 0 },
	m_viewport{ 0.0f, 0.0f, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0.0f, 1.0f },
	m_scissorRect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) },
	m_rtvDescriptorSize{ 0 }
{
	m_aspectRatio = static_cast<FLOAT>(width) / static_cast<FLOAT>(height);
}

GameFramework::~GameFramework()
{

}

void GameFramework::GameLoop()
{
	m_timer.Tick();
	if (m_isActive)
	{
		OnMouseEvent();
		OnKeyboardEvent();
	}
	OnUpdate();
	OnRender();
}

void GameFramework::OnInit(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	LoadPipeline();
	LoadAssets();
}

void GameFramework::OnUpdate()
{
	shared_ptr<Camera> camera{ m_scene->GetCamera() };
	shared_ptr<Player> player{ m_scene->GetPlayer() };
	Skybox* skybox{ m_scene->GetSkybox() };

	if (camera)
	{
		camera->UpdatePosition(m_timer.GetDeltaTime());
		if (skybox) skybox->SetPosition(camera->GetEye());
	}
	if (player)
	{
		player->Move(player->GetVelocity());
		player->ApplyFriction(m_timer.GetDeltaTime());
	}
}

void GameFramework::OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* ppCommandList[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	DX::ThrowIfFailed(m_swapChain->Present(1, 0));

	WaitForPreviousFrame();
}

void GameFramework::OnDestroy()
{
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
}

void GameFramework::OnMouseEvent() const
{
	if (m_scene) m_scene->OnMouseEvent(m_hWnd, m_width, m_height, m_timer.GetDeltaTime());
}

void GameFramework::OnKeyboardEvent() const
{
	if (m_scene) m_scene->OnKeyboardEvent(m_timer.GetDeltaTime());
}

void GameFramework::CreateDevice(const ComPtr<IDXGIFactory4>& factory)
{
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); ++i)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)))) break;
	}
	if (!m_device)
	{
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		DX::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	}
}

void GameFramework::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DX::ThrowIfFailed(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)));
}

void GameFramework::CreateSwapChain(const ComPtr<IDXGIFactory4>& factory)
{
	// 샘플링 수준 체크
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multiSampleQualityLevels;
	multiSampleQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	multiSampleQualityLevels.SampleCount = 4;
	multiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	multiSampleQualityLevels.NumQualityLevels = 0;
	m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &multiSampleQualityLevels, sizeof(multiSampleQualityLevels));
	m_MSAA4xQualityLevel = multiSampleQualityLevels.NumQualityLevels;

	// 스왑체인 생성
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_width;
	swapChainDesc.BufferDesc.Height = m_height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = m_MSAA4xQualityLevel > 1 ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_MSAA4xQualityLevel > 1 ? m_MSAA4xQualityLevel - 1 : 0;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 전체화면으로 전환할 때 적합한 디스플레이 모드를 선택

	ComPtr<IDXGISwapChain> swapChain;
	DX::ThrowIfFailed(factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, &swapChain));
	DX::ThrowIfFailed(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void GameFramework::CreateRtvDsvDescriptorHeap()
{
	// 렌더타겟뷰 서술자힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 깊이스텐실 서술자힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
}

void GameFramework::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FrameCount; ++i)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), NULL, rtvHandle);
		rtvHandle.ptr += m_rtvDescriptorSize;
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = m_width;
	resourceDesc.Height = m_height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = m_MSAA4xQualityLevel > 1 ? 4 : 1;
	resourceDesc.SampleDesc.Quality = m_MSAA4xQualityLevel > 1 ? m_MSAA4xQualityLevel - 1 : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	DX::ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&m_depthStencil)));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilViewDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GameFramework::CreateRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // t0

	CD3DX12_ROOT_PARAMETER rootParameter[3];
	rootParameter[0].InitAsConstants(16, 0, 0); // cbGameObject : 월드 변환 행렬(16)
	rootParameter[1].InitAsConstants(32, 1, 0); // cbCamera		: 뷰 변환 행렬(16) + 투영 변환 행렬(16)
	rootParameter[2].InitAsDescriptorTable(_countof(ranges), &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.Init(
		0,								 				// ShaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, 				// filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, 				// addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, 				// addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, 				// addressW
		0.0f,											// mipLODBias
		1,												// maxAnisotropy
		D3D12_COMPARISON_FUNC_ALWAYS,					// comparisonFunc
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,	// borderColor
		0.0f,											// minLOD
		D3D12_FLOAT32_MAX,								// maxLOD
		D3D12_SHADER_VISIBILITY_PIXEL,					// shaderVisibility
		0												// registerSpace
	);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameter), rootParameter, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature, error;
	DX::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	DX::ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void GameFramework::CreatePipelineStateObject()
{
	ComPtr<ID3DBlob> vertexShader, pixelShader;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), NULL, NULL, "VSMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), NULL, NULL, "PSMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));

	// 정점 셰이더 레이아웃 설정
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
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
	DX::ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void GameFramework::LoadPipeline()
{
	// 팩토리 생성
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	ComPtr<IDXGIFactory4> factory;
	DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	// 디바이스 생성
	CreateDevice(factory);

	// 명령큐 생성
	CreateCommandQueue();

	// 스왑체인 생성
	CreateSwapChain(factory);

	// 렌더타겟뷰, 깊이스텐실뷰의 서술자힙 생성
	CreateRtvDsvDescriptorHeap();

	// 렌더타겟뷰 생성
	CreateRenderTargetView();

	// 깊이스텐실뷰 생성
	CreateDepthStencilView();

	// 루트시그니쳐 생성
	CreateRootSignature();

	// 텍스쳐 컴파일, PSO 생성
	//CreatePipelineStateObject();

	// 명령할당자 생성
	DX::ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

	// 명령리스트 생성
	DX::ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));
	DX::ThrowIfFailed(m_commandList->Close());

	// 펜스 생성
	DX::ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_fenceValue = 1;

	// alt + enter 금지
	factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
}

void GameFramework::LoadAssets()
{
	// 명령을 추가할 것이기 때문에 Reset
	m_commandList->Reset(m_commandAllocator.Get(), NULL);

	// 씬 생성
	m_scene = make_unique<Scene>();

	// 메쉬 생성
	shared_ptr<CubeMesh> cubeMesh{ make_shared<CubeMesh>(m_device, m_commandList, 0.5f, 0.5f, 0.5f) };

	// 텍스쳐 생성
	shared_ptr<Texture> rockTexture{ make_shared<Texture>(m_device, m_commandList, TEXT("resource/rock.dds")) };
	shared_ptr<Texture> ceilTexture{ make_shared<Texture>(m_device, m_commandList, TEXT("resource/ceil.dds")) };
	shared_ptr<Texture> terrainTexture{ make_shared<Texture>(m_device, m_commandList, TEXT("resource/terrain.dds")) };

	// 셰이더 생성, 텍스쳐 설정
	shared_ptr<Shader> shader{ make_shared<Shader>(m_device, m_rootSignature) };

	// 지형 생성
	unique_ptr<HeightMapTerrain> terrain{
		make_unique<HeightMapTerrain>(m_device, m_commandList, TEXT("resource/heightMap.raw"), shader, terrainTexture, 257, 257, 25, 25, XMFLOAT3{ 0.5f, 0.1f, 0.5f })
	};
	terrain->SetPosition(XMFLOAT3{ -40.0f, -10.0f, -20.0f });
	m_scene->GetTerrain().push_back(move(terrain));

	// 스카이박스 생성
	unique_ptr<Skybox> skybox{ make_unique<Skybox>(m_device, m_commandList, m_rootSignature) };
	m_scene->SetSkybox(skybox);

	// 게임오브젝트 생성
	unique_ptr<GameObject> obj{ make_unique<GameObject>() };
	obj->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 5.0f });
	obj->SetMesh(cubeMesh);
	obj->SetShader(shader);
	obj->SetTexture(rockTexture);
	m_scene->GetGameObjects().push_back(move(obj));

	// 플레이어 생성
	shared_ptr<Player> player{ make_shared<Player>() };
	player->SetMesh(cubeMesh);
	player->SetShader(shader);
	player->SetTexture(ceilTexture);
	m_scene->SetPlayer(player);

	// 카메라 생성
	shared_ptr<ThirdPersonCamera> camera{ make_shared<ThirdPersonCamera>() };
	camera->SetEye(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	camera->SetAt(XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	camera->SetUp(XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	camera->SetPlayer(m_scene->GetPlayer());

	// 카메라 투영 행렬 설정
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, m_aspectRatio, 0.1f, 1000.0f));
	camera->SetProjMatrix(projMatrix);
	m_scene->SetCamera(camera);

	// 플레이어 카메라 설정
	m_scene->GetPlayer()->SetCamera(m_scene->GetCamera());

	// 명령 제출
	m_commandList->Close();
	ID3D12CommandList* ppCommandList[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	// 명령들이 완료될 때까지 대기
	WaitForPreviousFrame();

	// 디폴트 버퍼로의 복사가 완료됐으므로 업로드 버퍼를 해제한다.
	for (const auto& obj : m_scene->GetGameObjects())
		obj->ReleaseUploadBuffer();

	// 타이머 초기화
	m_timer.Tick();
}

void GameFramework::PopulateCommandList() const
{
	DX::ThrowIfFailed(m_commandAllocator->Reset());
	DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	// Set necessary state
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate that the back buffer will be used as a render target
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 렌더타겟, 깊이스텐실 버퍼 바인딩
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(m_frameIndex), m_rtvDescriptorSize };
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ m_dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	m_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);

	// 렌더타겟, 깊이스텐실 버퍼 지우기
	const FLOAT clearColor[]{ 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	// 렌더링
	if (m_scene) m_scene->Render(m_commandList);

	// Indicate back buffer will now be used to present
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	DX::ThrowIfFailed(m_commandList->Close());
}

void GameFramework::WaitForPreviousFrame()
{
	const UINT64 fence = m_fenceValue;
	DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
	++m_fenceValue;

	if (m_fence->GetCompletedValue() < fence)
	{
		DX::ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}