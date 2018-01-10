#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include <string>
#include <wrl.h>

#include "PerformanceTimer.h"

using Microsoft::WRL::ComPtr;

class EngineSample
{
public:
	EngineSample(HWND hWnd, int x, int y, int width, int height);
	void OnCreate();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	float aspectRatio_;
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

	HWND hWnd_;
	HWND hWndParent_;
	int x_ = 0, y_ = 0;
	UINT width_;
	UINT height_;
	bool useWarpDevice_ = false;

	PerformanceTimer timer_;
	float deltaTime_ = 0.0f;
	float totalTime_ = 0.0f;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};