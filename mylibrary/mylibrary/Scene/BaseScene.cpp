#include "BaseScene.h"
#include <cassert>

BaseScene::BaseScene()
{

}

void BaseScene::Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);
	assert(tMng);
	assert(dCamera);
	assert(bCamera);
	assert(gamepad);
	assert(dText);
	assert(fog);
	assert(post);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;
	this->tMng = tMng;
	this->debugCamera = dCamera;
	this->backCamera = bCamera;
	this->gamepad = gamepad;
	this->dText = dText;
	this->fog = fog;
	this->post = post;
}