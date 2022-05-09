#pragma once
#include "Camera.h"
class BackCamera :
    public Camera
{
public:
	/// <summary>
    /// XV
    /// </summary>
	void Update();

    void AddPhi(const float angle) override;

    void ResetCamera() override;
};

