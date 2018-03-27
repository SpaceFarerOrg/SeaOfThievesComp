#include "Application.h"

#ifdef _DEBUG
#pragma comment(lib, "sfml-network-d")
#pragma comment(lib, "PirateGameNetworkingCommon_Debug")
#else
#pragma comment(lib, "sfml-network")
#pragma comment(lib, "PirateGameNetworkingCommon_Release")
#endif

int main()
{
	CApplication app;
	app.Init();

	while (app.GetShouldRun())
	{
		app.Update();
	}

	return 0;
}