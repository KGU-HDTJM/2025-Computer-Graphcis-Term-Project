#pragma once

#include "define.h"

class Camera
{
public:
	Camera(const XMFLOAT4& eye, const XMFLOAT4& look, const XMFLOAT4& up);
	void Update(const XMFLOAT4& moveVector, float xDelta, float yDelta);
	XMMATRIX GetViewMatrix(void) const;
	const XMFLOAT4& GetPosition() const;
	void SetPosition(const XMFLOAT4& pos);
	const XMFLOAT4& GetViewVector() const;
public:
	XMFLOAT2 Sensitivity;
	float MovingSpeed;
private:
	XMFLOAT4 mPosition;
	XMFLOAT4 mViewVector;
	XMFLOAT4 mUp;
	XMMATRIX mViewTrans;
	float mPitch;
	float mYaw;
};


