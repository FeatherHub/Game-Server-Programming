#pragma once

#include <string>

class Logger
{
public:
	enum Level
	{
		INFO,
		WARN,
		ERR,
		DEFAULT
	};

	static void Write(Level lv, const char* fmt, ...);
};