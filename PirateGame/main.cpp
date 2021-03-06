#include <time.h>
#include <stdlib.h>
#include "Application.h"

#ifdef _DEBUG
#pragma comment (lib, "sfml-system-d")
#pragma comment (lib, "sfml-window-d")
#pragma comment (lib, "sfml-audio-d")
#pragma comment (lib, "sfml-graphics-d")
#pragma comment (lib, "sfml-network-d")
#else
#pragma comment (lib, "sfml-system")
#pragma comment (lib, "sfml-window")
#pragma comment (lib, "sfml-audio")
#pragma comment (lib, "sfml-graphics")
#pragma comment (lib, "sfml-network")
#endif

#include <iostream>
#include <random>
int main()
{
	CApplication app;
	app.Init();
	app.EnterMenu();

	while (app.GetShouldRun())
	{
		app.Update();
	}

	app.ShutDown();

	return 0;
}