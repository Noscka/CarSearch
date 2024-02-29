#pragma once

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HttpUtilities.hpp>

class PictureHolder
{
private:
	std::string PicURL;
	std::string PicFilePath;
public:
	PictureHolder(const std::string& picURL)
	{
		PicURL = picURL;
		NosLib::HttpUtilities::DownloadFile(picURL, "1.jpeg", false);
	}
};