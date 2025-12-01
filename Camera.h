#pragma once

#include "define.h"

class Camera
{
public:
	Camera(const XMFLOAT4& eye, const XMFLOAT4& look, const XMFLOAT4& up);
	void Update(const XMFLOAT4& moveVector, float xDelta, float yDelta);
	XMMATRIX GetViewMatrix(void) const;
	const XMFLOAT4& GetPosition() const;
private:
	XMFLOAT4 mPosition;
	XMFLOAT4 mViewVector;
	XMFLOAT4 mUp;
	XMMATRIX mViewTrans;
	float mPitch;
	float mYaw;
};


