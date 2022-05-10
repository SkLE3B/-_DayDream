#include "DirectXGame.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ゲームのインスタンス
	DirectXGame* Game = new DirectXGame();
	
	//ゲームの実行
	Game->Run();
	//解放
	delete Game;

	return 0;
}