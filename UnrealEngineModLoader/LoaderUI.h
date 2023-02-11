#pragma once
#include <Windows.h>
#include <wrl.h>
#include <inttypes.h>
#include <string>
#include <d3d11.h>
#include <d3d12.h>
#include <D3D12Shader.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_dx12.h"
#include "ImGui/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "libcmtd.lib")
#pragma comment(lib, "D3dcompiler.lib")

#define IMGUI_USER_CONFIG "ImGuiConfig.h"

using namespace Microsoft::WRL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

class LOADER_API LoaderUI
{
public:
	typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall* D3D12PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	D3D11PresentHook phookD3D11Present = NULL;
	D3D12PresentHook phookD3D12Present = NULL;

	DWORD_PTR* pSwapChainVtable = NULL;
	ID3D11Device* pDevice = NULL;
	ComPtr<ID3D12Device> p12Device = NULL;
	ID3D11DeviceContext* pContext = NULL;

	ComPtr<IDXGIFactory4> p12DXGIFactory;
	ComPtr<IDXGIAdapter> p12DXGIAdapter;

	ComPtr<ID3D12CommandQueue> p12CommandQueue = NULL;
	ComPtr<ID3D12CommandAllocator> p12CommandAllocator = NULL;
	ComPtr<ID3D12GraphicsCommandList> p12CommandList = NULL;
	ComPtr<IDXGIOutput> p12Output = NULL;

	WNDCLASSEX Hookwc;

	FrameContext p12frameContext;
	HANDLE p12hSwapChainWaitableObject = NULL;
	HANDLE p12fenceEvent = NULL;
	UINT64 p12fenceLastSignaledValue = 0;
	UINT p12frameIndex = 0;

	ComPtr<ID3D12DescriptorHeap> p12DescriptorHeapRtv = NULL;
	ComPtr<ID3D12DescriptorHeap> p12DescriptorHeapSrv = NULL;

	ComPtr<IDXGISwapChain3> pSwapChain;
	D3D12_CPU_DESCRIPTOR_HANDLE p12RenderTargetDescriptor;

	ComPtr<ID3D12Fence> p12Fence = NULL;

	ID3D11RenderTargetView* pRenderTargetView = NULL;
	ComPtr<ID3D12Resource> pRenderTarget;

	WNDPROC hGameWindowProc = NULL;

	D3D11_VIEWPORT viewport;
	D3D12_VIEWPORT viewport12;
	float screenCenterX = 0;
	float screenCenterY = 0;

	HRESULT(*ResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT) = NULL;

	HRESULT LoaderResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	void LoaderD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	void LoaderD3D12Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	static LRESULT CALLBACK hookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CleanupRenderTarget();

	void WaitForLastSubmittedFrame();

	FrameContext* WaitForNextFrameResources();

	void CleanupDeviceD3D();

	void CreateUILogicThread();

	static LoaderUI* GetUI();

	bool initRendering = true;

	static void HookDX();

	bool IsDXHooked = 0;

private:
	static LoaderUI* UI;
};

