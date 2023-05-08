#include "stdafx.h"

#include "App.h"

#include "AudioSink.h"

#include <Windows.h>
int main()
{
    //hide window if not debug
#ifndef _DEBUG
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif

    App app;
    app.run();

    _D("End of main, bye");
	return 0;
}