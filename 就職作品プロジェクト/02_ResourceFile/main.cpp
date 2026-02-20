#include "main.h"
#include "Application.h"
#include "JsonLoder.hpp"
#include "CSVLoader.h"
#include <algorithm>

int main(void)
{
	uint32_t appWidth = SCREEN_WIDTH;
	uint32_t appHeight = SCREEN_HEIGHT;

	AppConfig appConfig{};
	const std::filesystem::path configPath = SaveDir() / "AssetFile/Json/DebugConfig.json";
	if (ConfigIO::Load(configPath, appConfig))
	{
		appWidth = static_cast<uint32_t> (max(appConfig.window.width, 320));
		appHeight = static_cast<uint32_t>(max(appConfig.window.height, 240));
	}

	Application app(appWidth, appHeight);
	app.Run();

	return 0;
}