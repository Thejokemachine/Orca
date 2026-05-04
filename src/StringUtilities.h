#ifndef __ORCA_STRINGUTILITIES__
#define __ORCA_STRINGUTILITIES__

#include <string>
#include <vector>

namespace orca
{
	namespace utils
	{
		const std::string emptyString;

		void ReplaceAll(std::string_view source, std::string_view removeSequence, std::string_view replaceSequence, std::string& outBuffer);
		void SplitString(std::string_view source, std::string_view delimiter, std::vector<std::string>& outResults);
	}
}

#endif