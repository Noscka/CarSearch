#pragma once

#include <string>

class Listing
{
private:
	std::string URL;
	std::string Title;
	std::string Description;
	void* Pictures;

public:
	inline Listing(const std::string& title)
	{
		Title = title;
	}

	inline std::string GetTitle()
	{
		return Title;
	}
};