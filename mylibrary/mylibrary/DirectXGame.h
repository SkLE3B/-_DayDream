#pragma once
#include "GameBase.h"
#include "Scene/BaseScene.h"
#include "Scene/TitleScene.h"
#include "Scene/GameScene.h"
#include "Scene/EndScene.h"
#include "3d/ParticleManager.h"

class DirectXGame : public GameBase
{
public:
	DirectXGame* GetInstance();

public:
    void Initialize() override;

	virtual void Update() override;

	virtual void Draw() override;

	virtual void Finalize() override;

	void PostEffectSwitch(const std::string& sceneName);
private:
};