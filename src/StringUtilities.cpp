#include "StringUtilities.h"

#include <vector>

using namespace orca;

void utils::ReplaceAll(const std::string& source, const std::string& removeSequence, const std::string& replaceSequence, std::string& outBuffer)
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