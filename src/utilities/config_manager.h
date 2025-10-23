#pragma once
#include <json.hpp>

class config_manager
{
	public:
	config_manager();
	~config_manager();

	nlohmann::json m_config_data;

	void does_config_exist();
	void init_config();
	void load_config();
	void save_config();

	int get_theme();
	int set_theme(int theme_id);
};