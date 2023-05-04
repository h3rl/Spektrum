#include "stdafx.h"

#include "App.h"

#include "AudioSink.h"

#include <Windows.h>
int main()
{
    srand(time(0));

    try {
        App App;
        App.run();
    }
    catch (std::exception& e)
    {
        _D("\nEXCEPTION: " << e.what());
        return EXIT_FAILURE;
    }

    // end of application
	return EXIT_SUCCESS;
}