#pragma once

#include <iostream>
#include <sstream>

struct OutputSink
{
	OutputSink()
	{
		original = std::cout.rdbuf(buffer.rdbuf());
	}

	std::string content() const
	{
		return buffer.str();
	}

	~OutputSink()
	{
		std::cout.rdbuf(original);
	}

private:
	std::stringstream buffer;
	std::streambuf *original;
};
