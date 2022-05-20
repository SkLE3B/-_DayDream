#pragma once
#include "Object3d.h"

class TouchableObject :
	public Object3d
{
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	//static Boss* Create(Model* model = nullptr);
	static std::shared_ptr<TouchableObject> Create(Model* model = nullptr, Camera* camera = nullptr);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(Model* model,Camera* camera);
};