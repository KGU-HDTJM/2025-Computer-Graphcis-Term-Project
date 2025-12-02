#include "ColorTextureMap.h"

ColorTextureMap::~ColorTextureMap(void)
{
	delete mHSVColorPalette;
	mHSVTexture->Release();
	mHSVShaderRV->Release();
	mSamplerLinear->Release();
}
ID3D11Texture2D* ColorTextureMap::GetHSVTexture(void) const
{
	return mHSVTexture;
}

ID3D11ShaderResourceView* ColorTextureMap::GetHSVShaderRV(void) const
{
	return mHSVShaderRV;
}

ID3D11SamplerState* ColorTextureMap::GetSamplerLinear(void) const
{
	return mSamplerLinear;
}

// x: Red, y: Green, z: Blue
XMFLOAT4 ColorTextureMap::hsvToRGB(float hue, float saturation, float value) const
{
	float c = value * saturation;
	float x = c * (1 - fabsf(fmodf(hue / 60.F, 2.F) - 1));
	float m = value - c;
	float red;
	float green;
	float blue;

	if (hue < 60.F)
	{
		red = c;
		green = x;
		blue = 0;
	}
	else if (hue < 120.F)
	{
		red = x;
		green = c;
		blue = 0;
	}
	else if (hue < 180.F)
	{
		red = 0;
		green = c;
		blue = x;
	}
	else if (hue < 240.F) 
	{ 
		red = 0; 
		green = x; 
		blue = c; 
	}
	else if (hue < 300.F) 
	{ 
		red = x; 
		green = 0; 
		blue = c; 
	}
	else 
	{ 
		red = c; 
		green = 0; 
		blue = x; 
	}
	return XMFLOAT4(red + m, green + m, blue + m, 1.0F);
}

void ColorTextureMap::createHSVColorPalette(void)
{
	mHSVColorPalette = new vector<XMFLOAT4>(WIDTH * HEIGHT);

	for (int y = 0; y < HEIGHT; y++)
	{
		float s = (float)y / (HEIGHT - 1); // Saturation
		for (int x = 0; x < WIDTH; x++)
		{
			float h = (float)x / (WIDTH - 1) * 360.0f; // Hue
			XMFLOAT4 color = hsvToRGB(h, s, 1.0f); // Value = 1
			(*mHSVColorPalette)[y * WIDTH + x] = color;
		}
	}
}

void ColorTextureMap::uploadTexture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = WIDTH;
	desc.Height = HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = mHSVColorPalette->data();
	initData.SysMemPitch = WIDTH * sizeof(XMFLOAT4);

	HRESULT hr = device->CreateTexture2D(&desc, &initData, &mHSVTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed loading HSV Texture", L"Error", MB_OK);
	}
}

ID3D11ShaderResourceView* ColorTextureMap::createSRV(ID3D11Device* device, ID3D11Texture2D* texture)
{

	ID3D11ShaderResourceView* hsvSRV = nullptr;
	device->CreateShaderResourceView(mHSVTexture, nullptr, &hsvSRV);
	return hsvSRV;

}

void ColorTextureMap::createSamplerLinear(ID3D11Device* device)
{

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampDesc, &mSamplerLinear);
}
