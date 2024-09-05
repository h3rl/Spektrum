#include "config.h"
#include "logging.h"

#include <libconfig.h++>

using libconfig::FileIOException;
using libconfig::ParseException;
using libconfig::SettingNotFoundException;

using libconfig::Setting;

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
		log_err("Parse error at {} : {} - {}", pex.getFile(), pex.getLine(), pex.getError());
		return;
	}

	const Setting &root = cfg.getRoot();

	try
	{
		const Setting &audio = root["audio"];

		// audio.lookupValue("bar_count", )

		//         cout
		//     << setw(30) << left << "TITLE" << "  " << setw(30) << left << "AUTHOR" << "   " << setw(6) << left
		//     << "PRICE" << "  " << "QTY" << endl;

		// for (int i = 0; i < count; ++i)
		// {
		// 	const Setting &book = books[i];

		// 	// Only output the record if all of the expected fields are present.
		// 	string title, author;
		// 	double price;
		// 	int qty;

		// 	if (!(book.lookupValue("title", title) && book.lookupValue("author", author) &&
		// 	      book.lookupValue("price", price) && book.lookupValue("qty", qty)))
		// 		continue;

		// 	cout << setw(30) << left << title << "  " << setw(30) << left << author << "  " << '$' << setw(6) << right
		// 	     << price << "  " << qty << endl;
		// }
		// cout << endl;
	}
	catch (const SettingNotFoundException &nfex)
	{
		log_warn("Failed to get {}", nfex.getPath());
	}
}

void Save()
{
	// Save to config_file_name
}
}