#pragma once
#if !defined(D3D11_CORE)
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <EASTL/vector.h>

#pragma comment(lib, "dxgi.lib")  
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;
using namespace eastl;

ID3DInclude* const INCLUDE_HANDLER = D3D_COMPILE_STANDARD_FILE_INCLUDE;

class D3D11Base
{
public:
	D3D11Base(void) : mDevice(nullptr), mSwapChain(nullptr), mImmediateContext(nullptr)
		, mAdapter(nullptr), mFeatureLevel(D3D_FEATURE_LEVEL_11_1)
		, mRenderTargetView(nullptr), mDepthStencil(nullptr), mDepthStencilView(nullptr)
		, mVertexShaders(nullptr), mInputLayout(nullptr), mPixelShaders(nullptr)
	{
		mConstantBuffer.View = XMMatrixIdentity();
		mCBResize.Projection = XMMatrixIdentity();
		mCBFrame.World = XMMatrixIdentity();
	}
	~D3D11Base(void) {};
	bool Initialize(HWND hWnd);
	ID3D11Device* GetDevice(void) const;
	ID3D11DeviceContext* GetImmediateContext(void) const;
	bool AddVertexShader(const LPWSTR fileName, const UINT numElements = 0, const D3D11_INPUT_ELEMENT_DESC* layoutOrNULL = nullptr);
	bool AddPixelShader(const LPWSTR fileName);
	void Cleanup(void);

private:
	bool getMaxVideoMemoryAdapter(void);
	bool createDeviceAndSwapChain(HWND hWnd, UINT width, UINT height);
	bool createRenderTargets(UINT width, UINT height);
	void setFullSizeViewport(UINT width, UINT height);


private:
	ID3D11Device* mDevice = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11DeviceContext* mImmediateContext = nullptr;

	IDXGIAdapter1* mAdapter = nullptr;
	D3D_FEATURE_LEVEL mFeatureLevel;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	ID3D11InputLayout* mInputLayout = nullptr;
	vector<ID3D11VertexShader*>* mVertexShaders = nullptr;
	vector<ID3D11PixelShader*>* mPixelShaders = nullptr;

	ID3D11Buffer* mCBNeverChanges = nullptr;
	ID3D11Buffer* mCBChangeOnResize = nullptr;
	ID3D11Buffer* mCBChangesEveryFrame = nullptr;

	struct CBChangesEveryFrame
	{
		XMMATRIX World;
	} mCBFrame;

	struct CBNeverChanges
	{
		XMMATRIX View;
	} mConstantBuffer;

	struct CBChangeOnResize
	{
		XMMATRIX Projection;
	} mCBResize;

};

#endif
