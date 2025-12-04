#include "Camera.h"
#include <cfloat>

Camera::Camera(const XMFLOAT4& eye, const XMFLOAT4& look, const XMFLOAT4& up)
{
	mPosition = eye;
	XMVECTOR viewVec = XMLoadFloat4(&look) - XMLoadFloat4(&eye);
	viewVec = XMVector3Normalize(viewVec);
	XMStoreFloat4(&mViewVector, viewVec);
	mYaw = atan2f(mViewVector.x, mViewVector.z);
	float setur = mViewVector.y;
	if (setur > 1.F - FLT_EPSILON)
	{
		setur = 1.F;
	}
	if (setur < -1.F + FLT_EPSILON)
	{
		setur = -1.F;
	}
	mPitch = asinf(setur);
	mViewVector.w = 0.F;
	mUp = up;
	Sensitivity = XMFLOAT2(1.0F, 1.0F);
	MovingSpeed = 10.F;
	Update(XMFLOAT4(0, 0, 0, 0), 0, 0);
}

void Camera::Update(const XMFLOAT4& moveVector, float xDelta, float yDelta)
{
	if (fabsf(xDelta) + fabsf(yDelta) > FLT_EPSILON)
	{
		mYaw += xDelta * Sensitivity.x;
		mPitch += yDelta * Sensitivity.y;
	}
	const float LIMIT = XM_PIDIV2 - FLT_EPSILON;
	if (mPitch > LIMIT)
	{
		mPitch = LIMIT - FLT_EPSILON;
	}
	if (mPitch < -LIMIT)
	{
		mPitch = -LIMIT + FLT_EPSILON;
	}
	const float COS = cosf(mPitch);
	XMVECTOR forward = XMVectorSet(COS * sinf(mYaw), sinf(mPitch), COS * cosf(mYaw), 0.0F);
	XMVECTOR up = XMLoadFloat4(&mUp);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
	// up = XMVector3Normalize(XMVector3Cross(forward, right));
	XMVECTOR forwardMove = XMVector3Normalize(XMVector3Cross(right, up));

	XMVECTOR pos = XMLoadFloat4(&mPosition);
	XMVECTOR move = XMVectorZero();
	XMVECTOR xxx0 = XMVECTOR{ moveVector.x, moveVector.x, moveVector.x, 0.F };
	XMVECTOR yyy0 = XMVECTOR{ moveVector.y, moveVector.y, moveVector.y, 0.F };
	XMVECTOR zzz0 = XMVECTOR{ moveVector.z, moveVector.z, moveVector.z, 0.F };
	move = XMVectorMultiplyAdd(xxx0, right, move);
	move = XMVectorMultiplyAdd(yyy0, up, move);
	// move = XMVectorMultiplyAdd(yyy0, XMVECTOR{ 0.F, 1.F, 0.F, 0.F }, move);
	move = XMVectorMultiplyAdd(zzz0, forwardMove, move);
	// move = XMVectorMultiplyAdd(zzz0, XMVector3Cross(right, XMVECTOR{ 0.F, 1.F, 0.F, 0.F }), move);
	move = move * MovingSpeed;
	pos = XMVectorAdd(pos, move);

	XMVECTOR focus = XMVectorAdd(pos, forward);
	mViewTrans = XMMatrixLookAtLH(pos, focus, up);

	XMStoreFloat4(&mPosition, pos);
	XMStoreFloat4(&mViewVector, forward);
	XMStoreFloat4(&mUp, up);
}

XMMATRIX Camera::GetViewMatrix(void) const
{
	return mViewTrans;
}

const XMFLOAT4& Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetPosition(const XMFLOAT4& pos)
{
	mPosition = pos;
}

const XMFLOAT4& Camera::GetViewVector() const
{
	return mViewVector;
}
