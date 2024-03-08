#pragma once

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HostPath.hpp>

#include <string>
#include <format>
#include <atomic>

#include "PictureManager.hpp"

class Listing : public QObject
{
	Q_OBJECT

private:
	inline static int DirectoryId = 0; /* TODO: maybe think of some more advanced ID system */
	inline static std::mutex SetIncMutex;

	std::string URL;
	std::string Title;

	PictureManager PicManager;

signals:
	void AddSelfToUi(Listing*);

public:
	inline Listing(const std::string& title, const std::string& url, const std::vector<std::string>& picVector)
	{
		Title = title;
		URL = url;

		{ /* Prevent from threads using the same number and double incrementing */
			std::lock_guard<std::mutex> lock(SetIncMutex);
			PicManager.SetStoreDirectory(std::format("{}", DirectoryId));
			DirectoryId++;
		}
		PicManager.AddPicture(picVector);
	}

	inline void AddSelfToUiFunc()
	{
		emit AddSelfToUi(this);
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