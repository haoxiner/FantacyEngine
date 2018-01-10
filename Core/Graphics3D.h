#pragma once
#include "D3D12HelperFunctions.h"
class Graphics3D
{
public:
	Graphics3D();
	bool Initialize(HWND hWnd, int xResolution, int yResolution);
	void Render();
	void Destroy();
private:
	// D3D12
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
private:
	// D3D12
	static const UINT FrameCount = 2;
	ComPtr<IDXGISwapChain3> swapChain_;
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12DescriptorHeap> rtvDescHeap_;
	ComPtr<ID3D12PipelineState> pipelineState_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	UINT rtvDescSize_ = 0;
	UINT frameIndex_ = 0;
	HANDLE fenceEvent_;
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_;
	CD3DX12_VIEWPORT viewport_;
	bool useWarpDevice_ = false;
private:
	// properties
	int xResolution_;
	int yResolution_;
	float aspectRatio_;
	// low level variables
	HWND hWnd_;
};