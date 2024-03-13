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
		if (!std::filesystem::exists(PicFilePath)) /* if already deleted, just don't do anything */
		{
			return;
		}

		std::filesystem::remove(PicFilePath);
	}

	QImage MakeQImage()
	{
		QImage image;
		image.load(QString::fromStdString(PicFilePath));
		return image;
	}
};

class PictureManager
{
private:
	int PicIndex = 0;
	NosLib::DynamicArray<PictureHolder*> Pictures;

	std::string StoreDirectory;

	std::string MakeFileName()
	{
		return std::format("pic{}.jpeg", Pictures.GetItemCount());
	}

public:
	PictureManager() = default;

	~PictureManager()
	{
		if (!std::filesystem::exists(StoreDirectory)) /* if already deleted, just don't do anything */
		{
			return;
		}

		std::filesystem::remove_all(StoreDirectory);
	}

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

	void PreviousImage()
	{
		if (PicIndex <= 0)
		{
			return;
		}

		PicIndex--;
	}

	void NextImage()
	{
		if (PicIndex >= Pictures.GetLastArrayIndex())
		{
			return;
		}
		PicIndex++;
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

	QPixmap GetPixmap()
	{
		if (Pictures[PicIndex] == nullptr)
		{
			throw std::out_of_range("Got out of range");
		}

		return QPixmap::fromImage(Pictures[PicIndex]->MakeQImage());
	}
};