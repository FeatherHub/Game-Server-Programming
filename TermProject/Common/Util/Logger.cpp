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
			switch (*(fmt + 1))
			{
			case 's':
				std::cout << va_arg(args, const char*);

				fmt += 2;
				break;
			case 'd':
				std::cout << va_arg(args, int);

				fmt += 2;
				break;
			}			
		}
		else
		{
			std::cout << *fmt;

			++fmt;
		}
	}

	std::cout << std::endl;

	va_end(args);
}
