#include "BackCamera.h"

void BackCamera::Update()
{
	//カメラ自動移動	
	eye.z -= 0.5f;
	target.z -= 0.5f;

	Camera::Update();
}

void BackCamera::AddPhi(const float angle)
{
}

void BackCamera::ResetCamera()
{
}
