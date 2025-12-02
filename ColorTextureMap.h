#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <EASTL/vector.h>

using namespace DirectX;
using namespace eastl;

class ColorTextureMap
{
public:
	ColorTextureMap(ID3D11Device* device) : WIDTH(512), HEIGHT(512)
	{
		createHSVColorPalette();
		uploadTexture(device);
		mHSVShaderRV = createSRV(device, mHSVTexture);
		createSamplerLinear(device);
	}
	~ColorTextureMap(void);
	ID3D11Texture2D* GetHSVTexture(void) const;
	ID3D11ShaderResourceView* GetHSVShaderRV(void) const;
	ID3D11SamplerState* GetSamplerLinear(void) const;
private:
	XMFLOAT4 hsvToRGB(float hue, float saturation, float value) const;
	void createHSVColorPalette(void);
	void uploadTexture(ID3D11Device* device);
	ID3D11ShaderResourceView* createSRV(ID3D11Device* device, ID3D11Texture2D* texture);
	void createSamplerLinear(ID3D11Device* device);

public:
	const int WIDTH;
	const int HEIGHT;
private:
	vector<XMFLOAT4>* mHSVColorPalette;
	ID3D11Texture2D* mHSVTexture;
	ID3D11ShaderResourceView* mHSVShaderRV;
	ID3D11SamplerState* mSamplerLinear;
};

