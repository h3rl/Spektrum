#include "config.h"
#include "logging.h"

#include "beat_detector.h"
#include "window.h"

#include <libconfig.h++>

using libconfig::FileIOException;
using libconfig::ParseException;
using libconfig::SettingNotFoundException;

using libconfig::Setting;

#define LOAD_PARAM(setting, name, value)  \
	if (setting.exists(name))             \
	{                                     \
		setting.lookupValue(name, value); \
	}

namespace Config
{
const char *config_file_name = "config.cfg";

void Load()
{
	libconfig::Config cfg;

	// Read the file. If there is an error, report it and exit.
	try
	{
		cfg.readFile(config_file_name);
	}
	catch (const FileIOException &fioex)
	{
		log_warn("Could not load config.cfg file.");
		return;
	}
	catch (const ParseException &pex)
	{
		log_err("Failed to parse config at {} : {} - {}", pex.getFile(), pex.getLine(), pex.getError());
		//return;
	}

	Setting &root = cfg.getRoot();
	if (root.exists("graphics"))
	{
		LOAD_PARAM(root["graphics"], "vsync_enabled", Window::vsync);
		LOAD_PARAM(root["graphics"], "framerate_limit", Window::framerate_limit);
	}
	if (root.exists("audio"))
	{
		LOAD_PARAM(root["audio"], "dft_size", BeatDetector::dft_size);
		LOAD_PARAM(root["audio"], "dft_zeropad_factor", BeatDetector::dft_zeropad_factor);
		LOAD_PARAM(root["audio"], "filter_factor_sigma", BeatDetector::filter_factor_sigma);
		LOAD_PARAM(root["audio"], "time_smoothing", BeatDetector::time_smoothing);
		LOAD_PARAM(root["audio"], "db_min", BeatDetector::db_min);
		LOAD_PARAM(root["audio"], "db_max", BeatDetector::db_max);
		LOAD_PARAM(root["audio"], "frequency_min", BeatDetector::f_min);
		LOAD_PARAM(root["audio"], "frequency_max", BeatDetector::f_max);

		LOAD_PARAM(root["audio"], "loudness_alpha", BeatDetector::loudness_alpha);
		LOAD_PARAM(root["audio"], "loudness_f_min", BeatDetector::loudness_f_min);
		LOAD_PARAM(root["audio"], "loudness_f_max", BeatDetector::loudness_f_max);
	}
	// try
	// {
	// 	Setting &audio = root["audio"];
	// 	audio.lookupValue("dft_size", BeatDetector::dft_size);
	// }
	// catch (const SettingNotFoundException &nfex)
	// {
	// 	log_warn("Failed to get {} from config", nfex.getPath());
	// 	return;
	// }

	log_msg_ln("Config loaded successfully!");
}

void Save()
{
	libconfig::Config cfg;
	Setting &root = cfg.getRoot();

	if (!root.exists("graphics"))
	{
		root.add("graphics", Setting::Type::TypeGroup);
	}
	Setting &graphics = root["graphics"];
	graphics.add("vsync_enabled", Setting::Type::TypeBoolean) = Window::vsync;
	graphics.add("framerate_limit", Setting::Type::TypeInt) = (int)Window::framerate_limit;

	if (!root.exists("audio"))
	{
		root.add("audio", Setting::Type::TypeGroup);
	}
	Setting &audio = root["audio"];
	audio.add("dft_size", Setting::Type::TypeInt) = (int)BeatDetector::dft_size;
	audio.add("dft_zeropad_factor", Setting::Type::TypeFloat) = BeatDetector::dft_zeropad_factor;
	audio.add("filter_factor_sigma", Setting::Type::TypeFloat) = BeatDetector::filter_factor_sigma;
	audio.add("time_smoothing", Setting::Type::TypeFloat) = BeatDetector::time_smoothing;
	audio.add("db_min", Setting::Type::TypeFloat) = BeatDetector::db_min;
	audio.add("db_max", Setting::Type::TypeFloat) = BeatDetector::db_max;
	audio.add("frequency_min", Setting::Type::TypeFloat) = BeatDetector::f_min;
	audio.add("frequency_max", Setting::Type::TypeFloat) = BeatDetector::f_max;
	audio.add("loudness_alpha", Setting::Type::TypeFloat) = BeatDetector::loudness_alpha;
	audio.add("loudness_f_min", Setting::Type::TypeFloat) = BeatDetector::loudness_f_min;
	audio.add("loudness_f_max", Setting::Type::TypeFloat) = BeatDetector::loudness_f_max;

	try
	{
		cfg.writeFile(config_file_name);
		log_msg_ln("Config saved to: {}", config_file_name);
	}
	catch (const FileIOException &fioex)
	{
		log_err("Failed to write config");
	}
}
}