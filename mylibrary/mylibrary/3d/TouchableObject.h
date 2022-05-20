#pragma once
#include "Object3d.h"

class TouchableObject :
	public Object3d
{
public:
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	//static Boss* Create(Model* model = nullptr);
	static std::shared_ptr<TouchableObject> Create(Model* model = nullptr, Camera* camera = nullptr);

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize(Model* model,Camera* camera);
};