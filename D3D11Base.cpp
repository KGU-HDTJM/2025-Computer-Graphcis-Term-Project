#include "D3D11Base.h"

void* __cdecl operator new[](unsigned __int64 size, char const*, int, unsigned int, char const*, int)
{
	return new unsigned char[size];
}
void* __cdecl operator new[](unsigned __int64 size, unsigned __int64, unsigned __int64, char const*, int, unsigned int, char const*, int)
{
	return new unsigned char[size];
}
namespace eastl {

	void __cdecl AssertionFailure(void*, const char* expression) {
#ifdef _DEBUG
		OutputDebugStringA(expression);
		__debugbreak();
#else
#endif
	}
}

bool D3D11Base::Initialize(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	HRESULT hr = S_OK;

	if (!getMaxVideoMemoryAdapter())
	{
		goto LB_FAILED_GET_ADAPTER;
	}
	if (!createDeviceAndSwapChain(hWnd, width, height))
	{
		goto LB_FAILED_CREATE_DEVICE_AND_SWAP_CHAIN;
	}

	if (!createRenderTargets(width, height))
	{
		goto LB_FAILED_CREATE_RENDER_TARGETS;
	}

	if (!createConstBuffers(width, height))
	{
		goto LB_FAILED_CREATE_CONST_BUFFERS;
	}

	if (!createRasterizer())
	{
		goto LB_FAILED_CREATE_RASTER_STATE;
	}


	{
		// Default input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);	
		mVertexShaders = new vector<ID3D11VertexShader*>();
		mPixelShaders = new vector<ID3D11PixelShader*>();
		bool success = addVertexShader((const LPWSTR)L"default.vs.hlsl", numElements, layout);
		success = AddPixelShader((const LPWSTR)L"default.ps.hlsl");
	}

	setFullSizeViewport(width, height);

	return true;

LB_FAILED_CREATE_RASTER_STATE:
LB_FAILED_CREATE_CONST_BUFFERS:
LB_FAILED_CREATE_RENDER_TARGETS:
	mSwapChain->Release();
	mDevice->Release();
LB_FAILED_CREATE_DEVICE_AND_SWAP_CHAIN:
	mAdapter->Release();
LB_FAILED_GET_ADAPTER:;
	return false;
}

ID3D11Device* D3D11Base::GetDevice(void) const
{
	return mDevice;
}

ID3D11DeviceContext* D3D11Base::GetImmediateContext(void) const
{
	return mImmediateContext;
}

ID3D11RenderTargetView* D3D11Base::GetRenderTargetView(void) const {
	return mRenderTargetView;
}

ID3D11DepthStencilView* D3D11Base::GetDepthStencilView(void) const {
	return mDepthStencilView;
}

ID3D11Buffer* D3D11Base::GetChangeOnResizeBuffer(void) const {
	return mCBChangeOnResize;
}

ID3D11Buffer* D3D11Base::GetNeverChangeBuffer(void) const {
	return mCBNeverChanges;
}

ID3D11Buffer* D3D11Base::GetCBChangeEveryFrame(void) const
{
	return mCBChangesEveryFrame;
}

IDXGISwapChain* D3D11Base::GetSwapChain(void) const {
	return mSwapChain;
}

ID3D11RasterizerState* D3D11Base::GetRasterState(void) const {
	return mRasterState;
}

ID3DBlob* D3D11Base::CompileShader(const LPWSTR filePath, const LPCSTR entryPoint, const LPCSTR target) const
{
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pShaderBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(filePath, nullptr, INCLUDE_HANDLER,
		entryPoint, target, 0, 0, &pShaderBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "Error", MB_OK);
			pErrorBlob->Release();
		}
		if (pShaderBlob)
		{
			pShaderBlob->Release();
		}
		return nullptr;
	}
	if (pErrorBlob)
	{
		pErrorBlob->Release();
		pErrorBlob = nullptr;
	}
	return pShaderBlob;
}

bool D3D11Base::AddVertexShader(const LPWSTR filePath)
{
	return addVertexShader(filePath);
}

bool D3D11Base::addVertexShader(const LPWSTR filePath, const UINT numElements, const D3D11_INPUT_ELEMENT_DESC* layout)
{
	HRESULT hr;
	ID3DBlob* pVSBlob = CompileShader(filePath, "main", "vs_5_0");
	if (pVSBlob == nullptr) {
		return false;
	}
	ID3D11VertexShader* vertexShader = nullptr;
	mDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
	mVertexShaders->push_back(vertexShader);

	if (layout != nullptr) {
		hr = mDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &mInputLayout);
		if (FAILED(hr)) {
			pVSBlob->Release();
			return false;
		}
	}

	pVSBlob->Release();

	// Set the input layout
	mImmediateContext->IASetInputLayout(mInputLayout);
	return true;
}

bool D3D11Base::AddPixelShader(const LPWSTR filePath)
{
	HRESULT hr = NULL;
	ID3DBlob* pPSBlob = CompileShader(filePath, "main", "ps_5_0");
	ID3D11PixelShader* pixelShader = nullptr;
	if (pPSBlob == nullptr) {
		return false;
	}
	mDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
	mPixelShaders->push_back(pixelShader);
	
	if (pPSBlob)
	{
		pPSBlob->Release();
	}

	return true;
}

ID3D11VertexShader* D3D11Base::GetVertexShader(const eShaderID id) const
{
	return (*mVertexShaders)[(int)id];
}

ID3D11PixelShader* D3D11Base::GetPixelShader(const eShaderID id) const 
{
	return (*mPixelShaders)[(int)id];
}

void D3D11Base::OnResize(UINT width, UINT height)
{
	if (mDevice == nullptr || mSwapChain == nullptr)
	{
		return;
	}

	mImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	mImmediateContext->Flush();

	if (mDepthStencil) { mDepthStencil->Release(); }
	if (mDepthStencilView) { mDepthStencilView->Release(); }
	if (mRenderTargetView) { mRenderTargetView->Release(); }

	HRESULT hr;

	XMMATRIX projection = XMMatrixPerspectiveLH(XM_PIDIV4, (FLOAT)width / (FLOAT)height, 0.1f, 100.0f);

	hr = mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_SWAPCHAIN;
	}

	if (!createRenderTargets(width, height))
	{
		goto LB_FAILED_CREATE_RENDER_TARGETS;
	}

	mCBResize.Projection = XMMatrixTranspose(projection);
	mImmediateContext->UpdateSubresource(mCBChangeOnResize, 0, nullptr, &mCBResize, 0, 0);

	return;

LB_FAILED_CREATE_RENDER_TARGETS:
	MessageBoxA(nullptr, "render targets creation failure", "Error", MB_OK);
LB_FAILED_CREATE_SWAPCHAIN:
	MessageBoxA(nullptr, "swapchain creation failure", "Error", MB_OK);

	assert(false);
}

void D3D11Base::Cleanup(void)
{
	mImmediateContext->ClearState();
	for (auto i = mPixelShaders->begin(); i != mPixelShaders->end(); ++i)
	{
		(*i)->Release();
	}
	delete mPixelShaders;

	for (auto i = mVertexShaders->begin(); i != mVertexShaders->end(); ++i)
	{
		(*i)->Release();
	}
	delete mVertexShaders;

	mInputLayout->Release();

	mDepthStencilView->Release();
	mDepthStencil->Release();
	mRenderTargetView->Release();

	mImmediateContext->Release();
	mSwapChain->Release();
	mDevice->Release();
	mAdapter->Release();
}

bool D3D11Base::getMaxVideoMemoryAdapter(void)
{
	IDXGIFactory1* pFactory = nullptr;
	UINT adapterIdx = 0;

	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		return false;
	}


	DXGI_ADAPTER_DESC1 adapterDesc;
	size_t maxVideoMemSize = 0;
	IDXGIAdapter1* adapter;
	for (UINT i = 0; pFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.DedicatedVideoMemory > maxVideoMemSize)
		{
			adapterIdx = i;
			maxVideoMemSize = adapterDesc.DedicatedVideoMemory;
		}
		adapter->Release();
	}

	HRESULT hr = pFactory->EnumAdapters1(adapterIdx, &mAdapter);
	pFactory->Release();
	if (hr == DXGI_ERROR_NOT_FOUND)
	{
		return false;
	}
	return true;
}

bool D3D11Base::createDeviceAndSwapChain(HWND hWnd, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT createDeviceFlag = 0;
#if defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr = D3D11CreateDeviceAndSwapChain(mAdapter, D3D_DRIVER_TYPE_UNKNOWN,
		NULL, createDeviceFlag, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
		&sd, &mSwapChain, &mDevice, &mFeatureLevel, &mImmediateContext);

	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool D3D11Base::createConstBuffers(UINT width, UINT height)
{
	HRESULT hr;
	
	XMVECTOR eye = XMVectorSet(20.0f, 23.0f, -10.0f, 0.0f); 
	XMVECTOR at = XMVectorSet(20.0f, 23.0f, 0.0f, 0.0f);  
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eye, at, up);

	mConstantBuffer.View = XMMatrixTranspose(view);

	XMMATRIX projection = XMMatrixPerspectiveLH(XM_PIDIV4, (FLOAT)width / (FLOAT)height, 0.1f, 100.0f);
	mCBResize.Projection = XMMatrixTranspose(projection);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	
	bufferDesc.ByteWidth = sizeof(mConstantBuffer);
	initData.pSysMem = &mConstantBuffer;

	hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mCBNeverChanges);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREAET_NEVER_CHANGE_BUFFER;
	}

	bufferDesc.ByteWidth = sizeof(mCBResize);
	initData.pSysMem = &mCBResize;

	hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mCBChangeOnResize);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_CHANGE_ON_RESIZE_BUFFER;
	}

	bufferDesc.ByteWidth = sizeof(mCBFrame);

	hr = mDevice->CreateBuffer(&bufferDesc, nullptr, &mCBChangesEveryFrame);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_CHANGE_EVERY_FRAME_BUFFER;
	}

	mImmediateContext->UpdateSubresource(mCBChangeOnResize, 0, nullptr, &mCBResize, 0, 0);
	mImmediateContext->UpdateSubresource(mCBNeverChanges, 0, nullptr, &mConstantBuffer, 0, 0);

	return true;
LB_FAILED_CREATE_CHANGE_EVERY_FRAME_BUFFER:
	mCBChangeOnResize->Release();
LB_FAILED_CREATE_CHANGE_ON_RESIZE_BUFFER:
	mCBNeverChanges->Release();
LB_FAILED_CREAET_NEVER_CHANGE_BUFFER:
	return false;
}


bool D3D11Base::createRenderTargets(UINT width, UINT height)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	hr = mDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
	if (FAILED(hr))
	{
		pBackBuffer->Release();
		goto LB_FAILED_CREATE_RENDER_TARGET_VIEW;
	}

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mDevice->CreateTexture2D(&descDepth, nullptr, &mDepthStencil);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_DEPTH_STENCIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = mDevice->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView);
	if (FAILED(hr))
	{
		goto LB_FAILED_CREATE_DEPTH_STENCIL_VIEW;
	}
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	return true;
LB_FAILED_CREATE_DEPTH_STENCIL_VIEW:
	mDepthStencil->Release();
LB_FAILED_CREATE_DEPTH_STENCIL:
	mRenderTargetView->Release();
LB_FAILED_CREATE_RENDER_TARGET_VIEW:
	return false;
}

bool D3D11Base::createRasterizer(void) 
{
	HRESULT hr = NULL;

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;

	hr = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void D3D11Base::setFullSizeViewport(UINT width, UINT height)
{
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(vp));

	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.F;
	vp.MaxDepth = 1.0F;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &vp);
}
