#ifndef __ORCA_STRINGUTILITIES__
#define __ORCA_STRINGUTILITIES__

#include <string>
#include <algorithm>

namespace orca
{
	namespace utils
	{
		const std::string emptyString;

		void ReplaceAll(const std::string& source, const std::string& removeSequence, const std::string& replaceSequence, std::string& outBuffer);
	}
}

#endif