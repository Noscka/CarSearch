#pragma once

#include <NosLib/HostPath.hpp>

#include <string>

class Listing
{
private:
	std::string URL;
	std::string Title;
	std::string Description;
	void* Pictures;

public:
	inline Listing(const std::string& title, const std::string& description, const std::string& url)
	{
		Title = title;
		Description = description;
		URL = url;
	}

	inline std::string GetTitle() const
	{
		return Title;
	}

	inline std::string GetDescription() const
	{
		return Description;
	}

	inline std::string GetURL() const
	{
		return URL;
	}

private:
};