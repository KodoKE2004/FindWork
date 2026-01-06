#include "main.h"
#include "Application.h"

//=============================
//         エントリポイント
//=============================

int main(void)
{

	// アプリケーション実行
	Application app(SCREEN_WIDTH, SCREEN_HEIGHT);
	app.Run();

	return 0;
}