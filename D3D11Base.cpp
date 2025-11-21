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

	void AssertionFailure(const char* expression) {
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

	{
		// Default input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);
		mVertexShaders = new vector<ID3D11VertexShader*>();
		bool success = AddVertexShader((const LPWSTR)L"defaultVertexShader.hlsl", numElements, layout);
		mPixelShaders = new vector<ID3D11PixelShader*>();
		AddPixelShader((const LPWSTR)L"defaultPixelShader.hlsl");
	}


	return true;

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

bool D3D11Base::AddVertexShader(const LPWSTR fileName, const UINT numElements, const D3D11_INPUT_ELEMENT_DESC* layout)
{
	ID3DBlob* pErrorBlob = nullptr;

	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, nullptr, INCLUDE_HANDLER,
		"main", "vs_5_0", 0, 0, &pVSBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "Failed Compile Vertex Shader", MB_OK);
			pErrorBlob->Release();
		}
		if (pVSBlob)
		{
			pVSBlob->Release();
		}
		return false;
	}

	if (pErrorBlob)
	{
		pErrorBlob->Release();
		pErrorBlob = nullptr;
	}

	ID3D11VertexShader* vertexShader = nullptr;
	mDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
	mVertexShaders->push_back(vertexShader);


	hr = mDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &mInputLayout);


	pVSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}

	// Set the input layout
	mImmediateContext->IASetInputLayout(mInputLayout);
	return true;
}

bool D3D11Base::AddPixelShader(const LPWSTR fileName)
{
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pPSBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(fileName, nullptr, INCLUDE_HANDLER,
		"main", "ps_5_0", 0, 0, &pPSBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "Error", MB_OK);
			pErrorBlob->Release();
		}
		if (pPSBlob)
		{
			pPSBlob->Release();
		}
		return false;
	}
	ID3D11PixelShader* pixelShader = nullptr;
	mDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
	mPixelShaders->push_back(pixelShader);
	if (pPSBlob)
	{
		pPSBlob->Release();
	}
	if (pErrorBlob)
	{
		pErrorBlob->Release();
		pErrorBlob = nullptr;
	}

	return true;
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
	// ?
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
