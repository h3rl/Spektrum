#include "stdafx.h"

#include "App.h"

#include "AudioSink.h"

#include <Windows.h>
int main()
{
    //srand(time(0));

    //try {
    App App;
    App.run();
    //}
    //catch (std::exception& e)
    //{
    //    _D("\nEXCEPTION: " << e.what());
    //    return EXIT_FAILURE;
    //}

    _D("End of main, bye");

    // end of application
	return 0;
}