#pragma once
#include <string>
#include "BaseScene.h"

/// <summary>
/// シーン工場概念
/// </summary>
class AbstractSceneFactory
{
public:
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーン生成
	/// </summary>
	// <param name="sceneName">シーンネーム</param>
	/// <returns>生成したシーン</returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};