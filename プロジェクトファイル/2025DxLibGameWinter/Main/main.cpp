#include <DxLib.h>

#include "Library/System/Application.h"

using namespace std;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)  
{
	Application& app = Application::GetInstance();

	// アプリケーションの初期化
	if (!app.Init()) {
		return -1;
	}

	// メインループ
	app.Run();

	// 後処理
	app.Terminate();

	return 0;
}