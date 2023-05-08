#include "Config.h"

#include <fstream>

void Config::Save()
{
	// Save Config object to file
	std::fstream file;
	file.open("config.cfg", std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		file.write((char*)&CONFIG, sizeof(CONFIG));
		file.close();
	}
}

void Config::Load()
{
	// Load Config object from file
	std::fstream file;
	file.open("config.cfg", std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		// Check that file size is correct
		file.seekg(0, std::ios::end);
		int size = file.tellg();
		file.seekg(0, std::ios::beg);
		if (size != sizeof(CONFIG))
		{
			file.close();
			std::thread t([]()
			{
				MessageBoxA(NULL, "Config file size is incorrect!\nYou should delete it..\nusing default config", "Error", MB_OK);
			});
			t.detach();
			return;
		}

		file.read((char*)&CONFIG, sizeof(CONFIG));
		file.close();
	}
}
