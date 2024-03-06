#pragma once

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HostPath.hpp>

#include <string>
#include <format>

#include "PictureManager.hpp"

class Listing
{
private:
	inline static int DirectoryId = 0; /* TODO: maybe think of some more advanced ID system */

	std::string URL;
	std::string Title;

	PictureManager PicManager;

public:
	inline Listing(const std::string& title, const std::string& url, const std::vector<std::string>& picVector)
	{
		Title = title;
		URL = url;

		PicManager.SetStoreDirectory(std::format("{}", DirectoryId));
		PicManager.AddPicture(picVector);

		DirectoryId++;
	}

	inline std::string GetTitle() const
	{
		return Title;
	}

	inline std::string GetURL() const
	{
		return URL;
	}

	inline PictureManager* GetPictureManager()
	{
		return &PicManager;
	}

private:
};