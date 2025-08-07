#pragma once
#include <iostream>

#include "../utilities/filtermanager.h"

class ControllerFilterManager
{
public:
	ControllerFilterManager();
	~ControllerFilterManager();

	FilterManager* filterManager;
};