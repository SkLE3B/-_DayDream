#pragma once
#include <string>
#include "BaseScene.h"

/// <summary>
/// �V�[���H��T�O
/// </summary>
class AbstractSceneFactory
{
public:
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// �V�[������
	/// </summary>
	// <param name="sceneName">�V�[���l�[��</param>
	/// <returns>���������V�[��</returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};