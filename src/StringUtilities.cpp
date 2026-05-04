#include "StringUtilities.h"

#include <vector>

using namespace orca;

void utils::ReplaceAll(std::string_view source, std::string_view removeSequence, std::string_view replaceSequence, std::string& outBuffer)
{
	const size_t stride = removeSequence.length();
	std::vector<size_t> foundInstances;
	size_t pos = source.find(removeSequence);
	while (pos != std::string::npos)
	{
		foundInstances.emplace_back(pos);
		pos = source.find(removeSequence, pos + stride);
	}

	if (!foundInstances.empty())
	{
		size_t prevIdx = 0;
		for (size_t idx : foundInstances)
		{
			size_t step = idx - prevIdx;
			outBuffer.append(source.substr(prevIdx, step));
			outBuffer.append(replaceSequence);
			prevIdx += step + stride;
		}
		if (prevIdx < source.length())
		{
			outBuffer.append(source.substr(prevIdx));
		}
	}
	else
	{
		outBuffer = source;
	}
}

void utils::SplitString(std::string_view source, std::string_view delimiter, std::vector<std::string>& outResults)
{
	size_t prevPos = 0;
	size_t pos = source.find(delimiter);
	while (pos != std::string::npos)
	{
		size_t length = pos - prevPos;
		std::string s(source.substr(prevPos, length));
		outResults.emplace_back(s);
		prevPos = pos+1;
		pos = source.find(delimiter, pos + delimiter.length());
	}

	outResults.emplace_back(source.substr(prevPos));
}
