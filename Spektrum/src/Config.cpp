#include "Config.h"

#include <fstream>

const std::string CONFIG_NAME = "config.cfg";

namespace state
{
    extern bool window_needs_redraw{ false };
    extern bool in_gui{ false };

    extern bool window_show_fps{ true };
    extern bool window_show_menu{ true };
    extern bool window_show_config{ false };
}

namespace config
{
    extern int window_width{ 1600 };
    extern int window_height{ 900 };

    extern bool load_config_on_startup{ true };

    namespace graphics
    {
        extern bool vsync{ false };
        extern int framerate_limit{ 240 };
        extern int antialiasing_level{ 4 };
    }
    namespace audio
    {
        extern AudioWindowFunction windowfunction{ Blackman };
        extern AudioAxisScale barstyle{ Logarithmic };
        extern int bar_count{ (int)(FFT_SIZE_HALF) };
        extern float bar_gain{ 1.0f };
        extern float time_smoothing{ 0.82f };
        extern float min_db{ -60.0f };
        extern float max_db{ 0.0f };
        extern float min_freq{ 20.0f };
        extern float max_freq{ 20000.0f };
    }


    /**********************************************/
    /* Save and Load Config                        */
    /**********************************************/

#define CONFIG_SAVE(variable) file << #variable << " " << variable << std::endl;
#define CONFIG_SAVE_ENUM(variable, type) file << #variable << " " << static_cast<int>(variable) << std::endl;
    void Save()
    {
        std::fstream file(CONFIG_NAME, std::ios::out | std::ios::binary);

        if (!file.is_open())
        {
            _D("Could not create config file");
            return;
        }

        CONFIG_SAVE(window_width);
        CONFIG_SAVE(window_height);

        CONFIG_SAVE(load_config_on_startup);

        CONFIG_SAVE(graphics::vsync);
        CONFIG_SAVE(graphics::framerate_limit);
        CONFIG_SAVE(graphics::antialiasing_level);

        CONFIG_SAVE_ENUM(audio::windowfunction, AudioWindowFunction);
        CONFIG_SAVE_ENUM(audio::barstyle, AudioBarStyle);
        CONFIG_SAVE(audio::bar_count);
        CONFIG_SAVE(audio::bar_gain);
        CONFIG_SAVE(audio::time_smoothing);
        CONFIG_SAVE(audio::min_db);
        CONFIG_SAVE(audio::max_db);
        CONFIG_SAVE(audio::min_freq);
        CONFIG_SAVE(audio::max_freq);
    }
#undef CONFIG_SAVE
#undef CONFIG_SAVE_ENUM
#define CONFIG_LOAD(variable) if (key == #variable) { iss >> variable; }
#define CONFIG_LOAD_ENUM(variable, type) if (key == #variable) { int value; iss >> value; variable = static_cast<type>(value); }

    void Load()
    {
        std::fstream file(CONFIG_NAME, std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            _D("Could not open config file");
            return;
        }

        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, ' '))
            {
                CONFIG_LOAD(window_width);
                CONFIG_LOAD(window_height);

                CONFIG_LOAD(load_config_on_startup);

                CONFIG_LOAD(graphics::vsync);
                CONFIG_LOAD(graphics::framerate_limit);
                CONFIG_LOAD(graphics::antialiasing_level);

                CONFIG_LOAD_ENUM(audio::windowfunction, AudioWindowFunction);
                CONFIG_LOAD_ENUM(audio::barstyle, AudioAxisScale);
                CONFIG_LOAD(audio::bar_count);
                CONFIG_LOAD(audio::bar_gain);
                CONFIG_LOAD(audio::time_smoothing);
                CONFIG_LOAD(audio::min_db);
                CONFIG_LOAD(audio::max_db);
                CONFIG_LOAD(audio::min_freq);
                CONFIG_LOAD(audio::max_freq);
            }
        }
        file.close();

        // clamp values
        audio::bar_count = clamp(audio::bar_count, 1, (int)(FFT_SIZE_HALF*0.6));
        audio::bar_gain = clamp(audio::bar_gain, 0.0f, 10.0f);
        audio::time_smoothing = clamp(audio::time_smoothing, 0.0f, 1.0f);
        audio::max_db = clamp(audio::max_db, audio::min_db, 25.0f);
        audio::min_db = clamp(audio::min_db, -std::numeric_limits<float>::infinity(), audio::max_db);
        audio::max_freq = clamp(audio::max_freq, audio::min_freq, 20000.0f);
        audio::min_freq = clamp(audio::min_freq, 0.0f, audio::max_freq);

        // request redraw
        state::window_needs_redraw = true;

        _D("Config loaded");
    }

    bool LoadOnStartup()
    {

        std::fstream file(CONFIG_NAME, std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            _D("Could not open config file");
            return false;
        }

        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, ' '))
            {
                CONFIG_LOAD(load_config_on_startup);
            }
        }
        file.close();

        return load_config_on_startup;
    }

#undef CONFIG_LOAD
#undef CONFIG_LOAD_ENUM
}

