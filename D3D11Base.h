#pragma once
#if !defined(D3D11_CORE)
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <EASTL/vector.h>

#include "define.h"

#pragma comment(lib, "dxgi.lib")  
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;
using namespace eastl;

ID3DInclude* const INCLUDE_HANDLER = D3D_COMPILE_STANDARD_FILE_INCLUDE;

enum class eShaderID
{
	Basic,
	Count
};

class D3D11Base
{
public:
	D3D11Base(void) : mDevice(nullptr), mSwapChain(nullptr), mImmediateContext(nullptr)
		, mAdapter(nullptr), mFeatureLevel(D3D_FEATURE_LEVEL_11_1)
		, mRenderTargetView(nullptr), mDepthStencil(nullptr), mDepthStencilView(nullptr)
		, mVertexShaders(nullptr), mInputLayout(nullptr), mPixelShaders(nullptr)
	{
		mCBResize.Projection = XMMatrixIdentity();
	}
	~D3D11Base(void) {};
	bool Initialize(HWND hWnd);
	bool AddVertexShader(const LPWSTR filePath);
	bool AddPixelShader(const LPWSTR filePath);
	void OnResize(HWND hWnd);
	void Cleanup(void);
	
	ID3D11Device* GetDevice(void) const;
	ID3D11DeviceContext* GetImmediateContext(void) const;
	ID3D11RenderTargetView* GetRenderTargetView(void) const;
	ID3D11DepthStencilView* GetDepthStencilView(void) const;
	ID3D11VertexShader* GetVertexShader(const eShaderID id) const;
	ID3D11PixelShader* GetPixelShader(const eShaderID id) const;
	ID3D11Buffer* GetCBFrameBuffer(void) const;
	ID3D11Buffer* GetCBResizeBuffer(void) const;
	ID3D11Buffer* GetCBObjectBuffer(void) const;
	IDXGISwapChain* GetSwapChain(void) const;
	ID3D11RasterizerState* GetRasterState(void) const;
	ID3DBlob* CompileShader(const LPWSTR filePath, /*const LPCSTR entryPoint,*/ const LPCSTR target) const;
	ID3D11InputLayout* GetInputLayout(void) const;

private:
	bool getMaxVideoMemoryAdapter(void);
	bool createDeviceAndSwapChain(HWND hWnd, UINT width, UINT height);
	bool createRenderTargets(UINT width, UINT height);
	bool createConstBuffers(UINT width, UINT height);
	bool createRasterizer(void);
	void setFullSizeViewport(UINT width, UINT height);
	bool addVertexShader(const LPWSTR filePath, const UINT numElements = 0, const D3D11_INPUT_ELEMENT_DESC* layoutOrNULL = nullptr);

private:
	ID3D11Device* mDevice = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11DeviceContext* mImmediateContext = nullptr;

	IDXGIAdapter1* mAdapter = nullptr;
	D3D_FEATURE_LEVEL mFeatureLevel;
	ID3D11RasterizerState* mRasterState = nullptr;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	ID3D11InputLayout* mInputLayout = nullptr;
	vector<ID3D11VertexShader*>* mVertexShaders = nullptr;
	vector<ID3D11PixelShader*>* mPixelShaders = nullptr;

	ID3D11Buffer* mCBResizeBuffer = nullptr;
	ID3D11Buffer* mCBFrameBuffer = nullptr;
	ID3D11Buffer* mCBObjectBuffer = nullptr;

	CBResize mCBResize;
	
};

#endif
