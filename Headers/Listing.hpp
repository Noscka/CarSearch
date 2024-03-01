#pragma once

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HostPath.hpp>

#include <string>
#include <format>

#include "PictureManager.hpp"

class Listing
{
private:
	inline static int DirectoryId = 0;

	std::string URL;
	std::string Title;
	std::string Description;

	PictureManager PicManager;

public:
	inline Listing(const std::string& title, const std::string& description, const std::string& url, const std::vector<std::string>& picVector)
	{
		Title = title;
		Description = description;
		URL = url;

		PicManager.SetStoreDirectory(std::format("{}{}", Title[0], DirectoryId));
		PicManager.AddPicture(picVector);

		DirectoryId++;
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