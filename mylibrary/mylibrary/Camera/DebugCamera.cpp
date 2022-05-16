#include "DebugCamera.h"

using namespace DirectX;

DebugCamera::DebugCamera()
{
	phi = -3.14159265f / 2;
	theta = 0.2f;
}

void DebugCamera::Initialize(int window_width, int window_height, Input* input)
{
	//基底クラスの初期化を明示的に呼び出す
	Camera::Initialize(window_width, window_height);

	assert(input);

	this->input = input;
	// 画面サイズに対する相対的なスケールに調整
	scaleX = 1.0f / (float)window_width;
	scaleY = 1.0f / (float)window_height;	
	phi = -3.14159265f / 2;
	theta = 0.2f;
}

void DebugCamera::Update()
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	Input::MouseMove mouseMove = input->GetMouseMove();

	 //マウスの左ボタンが押されていたらカメラを回転させる
	//if (input->PushMouseLeft())
	//{
	//	phi += XM_PI / 180.0f * mouseMove.lX / 7;
	//	theta += XM_PI / 180.0f * mouseMove.lY / 7;
	//	if (theta > 40 * XM_PI / 180.0f)
	//		theta = 40 * XM_PI / 180.0f;
	//	else if (theta < -40 * XM_PI / 180.0f)
	//		theta = -40 * XM_PI / 180.0f;

	//	if (phi > 360 * XM_PI / 180.0f)
	//		phi -= 360 * XM_PI / 180.0f;
	//	else if (phi < 0)
	//		phi += 360 * XM_PI / 180.0f;

	//	dirty = true;
	//}

	//if (input->IsPush(DIK_UP))
	//{
	//	theta -= XM_PI / 180.0f * cameraMoveSpeed / 7;
	//	if (theta > 40 * XM_PI / 180.0f)
	//		theta = 40 * XM_PI / 180.0f;
	//	else if (theta < -40 * XM_PI / 180.0f)
	//		theta = -40 * XM_PI / 180.0f;
	//	dirty = true;
	//}
	//else if(input->IsPush(DIK_DOWN))
	//{
	//	theta += XM_PI / 180.0f * cameraMoveSpeed / 7;
	//	if (theta > 40 * XM_PI / 180.0f)
	//		theta = 40 * XM_PI / 180.0f;
	//	else if (theta < -40 * XM_PI / 180.0f)
	//		theta = -40 * XM_PI / 180.0f;
	//	dirty = true;
	//}

	if (input->IsPush(DIK_RIGHT))
	{
		phi -= XM_PI / 180.0f * cameraMoveSpeed / 7;
		if (phi > 360 * XM_PI / 180.0f)
			phi -= 360 * XM_PI / 180.0f;
		else if (phi < 0)
			phi += 360 * XM_PI / 180.0f;

		dirty = true;
	}

	if (input->IsPush(DIK_LEFT))
	{
		phi += XM_PI / 180.0f * cameraMoveSpeed / 7;
		if (phi > 360 * XM_PI / 180.0f)
			phi -= 360 * XM_PI / 180.0f;
		else if (phi < 0)
			phi += 360 * XM_PI / 180.0f;

		dirty = true;
	}

	if (dirty || viewDirty) {
		float nowTheta = theta;
		if (theta < 0)
			nowTheta = 0;
		eye = Vector3(cos(phi) * cos(nowTheta), sin(nowTheta), sin(phi) * cos(nowTheta)) * distance + target;
		viewDirty = true;
	}

	Camera::Update();
}

void DebugCamera::AddPhi(const float angle)
{
	phi += angle;
	if (phi > 360 * XM_PI / 180.0f)
		phi -= 360 * XM_PI / 180.0f;
	else if (phi < 0)
		phi += 360 * XM_PI / 180.0f;

	viewDirty = true;
}

void DebugCamera::ResetCamera()
{
	phi = -3.14159265f / 2;
	theta = 0.2f;
}