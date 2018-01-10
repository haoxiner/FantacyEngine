#include "EngineSample.h"

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr)) {
		throw std::exception();
	}
}

EngineSample::EngineSample(HWND hWnd, int x, int y, int width, int height):
	hWndParent_(hWnd),x_(x), y_(y), width_(width),height_(height),
	viewport_(x,y,width,height)
{
	aspectRatio_ = static_cast<float>(width) / static_cast<float>(height);
}

void EngineSample::OnCreate()
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent_, GWL_HINSTANCE);
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"DXSampleClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	hWnd_ = CreateWindow(
		windowClass.lpszClassName,
		L"Fantacy Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,		// We have no parent window.
		nullptr,		// We aren't using menus.
		hInstance,
		this); //TODO: pass render system to lparam in the future.

	LoadPipeline();
	LoadAssets();

	ShowWindow(hWnd_, SW_SHOW);

	// Main sample loop.
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	
}

// Main message handler for the sample.
LRESULT CALLBACK EngineSample::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	EngineSample* pSample = reinterpret_cast<EngineSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message) {
	case WM_CREATE:
	{
		// Save the DXSample* passed in to CreateWindow.
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION);
		//SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
	}
	return 0;

	case WM_KEYDOWN:
		if (pSample) {
			//pSample->OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_KEYUP:
		if (pSample) {
			//pSample->OnKeyUp(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_PAINT:
		if (pSample) {
			pSample->OnUpdate();
			pSample->OnRender();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}


void EngineSample::OnUpdate()
{
	deltaTime_ = timer_.Tick();
	totalTime_ += deltaTime_;
}

void EngineSample::OnRender()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(swapChain_->Present(1, 0));

	WaitForPreviousFrame();
}

void EngineSample::OnDestroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	WaitForPreviousFrame();

	CloseHandle(fenceEvent_);
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
static void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
			break;
		}
	}

	*ppAdapter = adapter.Detach();
}

void EngineSample::LoadPipeline()
{
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif //  defined(_DEBUG)
	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (useWarpDevice_) {
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device_)
		));
	} else {
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device_)
		));
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_)));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width_;
	swapChainDesc.Height = height_;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		commandQueue_.Get(),		// Swap chain needs the queue so that it can force a flush on it.
		hWnd_,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(factory->MakeWindowAssociation(hWnd_, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&swapChain_));
	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescHeap_)));

		rtvDescSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescHeap_->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++) {
			ThrowIfFailed(swapChain_->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
			device_->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescSize_);
		}
	}

	ThrowIfFailed(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_)));
}

void EngineSample::LoadAssets()
{
	// Create the command list.
	ThrowIfFailed(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(commandList_->Close());

	// Create synchronization objects.
	{
		ThrowIfFailed(device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
		fenceValue_ = 1;

		// Create an event handle to use for frame synchronization.
		fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent_ == nullptr) {
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

void EngineSample::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(commandAllocator_->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(commandList_->Reset(commandAllocator_.Get(), pipelineState_.Get()));

	// Set necessary state
	commandList_->RSSetViewports(1, &viewport_);


	// Indicate that the back buffer will be used as a render target.
	commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex_].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescHeap_->GetCPUDescriptorHandleForHeapStart(), frameIndex_, rtvDescSize_);

	// Record commands.
	const float clearColor[] = { 0.4f, 0.0f, std::sinf(totalTime_), 1.0f };
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Indicate that the back buffer will now be used to present.
	commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex_].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(commandList_->Close());
}

void EngineSample::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = fenceValue_;
	ThrowIfFailed(commandQueue_->Signal(fence_.Get(), fence));
	fenceValue_++;

	// Wait until the previous frame is finished.
	if (fence_->GetCompletedValue() < fence) {
		ThrowIfFailed(fence_->SetEventOnCompletion(fence, fenceEvent_));
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();
}
