#pragma once
class Config
{
public:
    static Config& get()
    {
        static Config instance;
        return instance;
    }

	struct _Window
	{
		int width{ 600 };
		int height{ 400 };
		float zoom{ 1.0f };
		int max_fps = 165;
	} window;

	struct _App
	{
		float speed{ 1.0f };
		float particle_max_velocity{ 100.0f };
		float gravity_constant{ 5.0f };
	} App;

	struct _Config
	{
		bool load{ false };
		bool save{ false };
	}config;

	bool in_gui{ false };

    Config() {};
    Config(Config const&) = delete;
    void operator=(Config const&) = delete;
};

#define CONFIG Config::get()