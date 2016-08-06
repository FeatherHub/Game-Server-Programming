#include "Logger.h"
#include <iostream>
#include <cstdarg>

void Logger::Write(Level lv, const char* fmt, ...)
{
	switch (lv)
	{
	case Logger::INFO:
		std::cout << "[INFO] ";
		break;
	case Logger::WARN:
		std::cout << "[WARN] ";
		break;
	case Logger::ERR:
		std::cout << "[ERROR] ";
		break;
	}

	va_list args;
	va_start(args, fmt);

	while (*fmt != '\0')
	{
		if (*fmt == '%')
		{
			if (*(fmt + 1) == 's')
			{
				std::cout << va_arg(args, const char*);

				fmt += 2;
			}
		}
		else
		{
			std::cout << *fmt;

			++fmt;
		}
	}

	va_end(args);
}
