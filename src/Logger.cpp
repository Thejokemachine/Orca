#include "Logger.h"

using namespace orca;

void CLogger::Log(const std::string& output)
{
	std::puts(output.c_str());
}