#pragma once

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HttpUtilities.hpp>

#include <filesystem>
#include <format>

class PictureHolder
{
private:
	std::string PicURL;
	std::string PicFilePath;
public:
	PictureHolder(const std::string& picURL, const std::string& downloadPath)
	{
		PicURL = picURL;
		PicFilePath = downloadPath;
		NosLib::HttpUtilities::DownloadFile(picURL, downloadPath, false);
	}

	~PictureHolder()
	{
		std::filesystem::remove(PicFilePath);
	}
};

class PictureManager
{
private:
	NosLib::DynamicArray<PictureHolder*> Pictures;

	std::string StoreDirectory;

	std::string MakeFileName()
	{
		return std::format("pic{}.jpeg", Pictures.GetItemCount());
	}
public:
	PictureManager() = default;

	void SetStoreDirectory(const std::string& storeDirectory)
	{
		StoreDirectory = storeDirectory;

		if (StoreDirectory.back() != '/' || StoreDirectory.back()!='\\')
		{
			StoreDirectory += "/";
		}

		if (!std::filesystem::exists(StoreDirectory))
		{
			std::filesystem::create_directory(StoreDirectory);
		}
	}

	void AddPicture(const std::string& picURL)
	{
		std::string filePath = StoreDirectory + MakeFileName();

		Pictures.Append(new PictureHolder(picURL, filePath));
	}

	void AddPicture(const std::vector<std::string>& urlArray)
	{
		for (std::string picEntry : urlArray)
		{
			AddPicture(picEntry);
		}
	}
};