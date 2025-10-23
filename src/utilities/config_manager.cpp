#include "config_manager.h"

#include <iostream>
#include <fstream>
#include <string>

config_manager::config_manager()
{
}

config_manager::~config_manager()
{
}

void config_manager::does_config_exist()
{
}

void config_manager::init_config()
{
	std::ofstream config_file("config.json");
	if (!config_file.is_open())
	{
		std::cerr << "Error creating config file." << std::endl;
		return;
	}

	m_config_data["theme"] = "0";

	std::cout << "Creating default config file." << std::endl;
	config_file << m_config_data.dump(4);
}

void config_manager::load_config()
{
}

void config_manager::save_config()
{
}
