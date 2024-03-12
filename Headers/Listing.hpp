#pragma once

#include <QObject>
#include <QDesktopServices>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/HostPath.hpp>

#include <string>
#include <format>
#include <atomic>

#include "PictureManager.hpp"

class Price
{
public:
	enum Currency
	{
		GBP,
		ZLOTY,
		USD
	};

protected:
	uint64_t Amount;
	Currency CurrencyType;

	std::string CurrencyToString()
	{
		switch (CurrencyType)
		{
		case GBP:
			return "GBP";
		case ZLOTY:
			return "zł";
		case USD:
			return "USD";
		}
	return "UNKNOWN";
	}

public:
	Price() = default;

	Price(uint64_t amount, Currency currencyType)
	{
		Amount = amount;
		CurrencyType = currencyType;
	}

	uint64_t GetPrice(Currency currencyType)
	{
		if (CurrencyType == CurrencyType)
		{
			return Amount;
		}
		/* ELSE */

		return 0; /* TODO: DO CONVERSION HERE LATER */
	}

	uint64_t GetPrice()
	{
		return GetPrice(CurrencyType);
	}

	std::string str()
	{
		return std::format("{} {}", Amount, CurrencyToString());
	}
};

class Listing : public QObject /* Add check for offer */
{
	Q_OBJECT

public:
	enum class Type : bool
	{
		Normal,
		Auction
	};

private:
	inline static int DirectoryId = 0; /* TODO: maybe think of some more advanced ID system */
	inline static std::mutex SetIncMutex;

	std::string URL;
	std::string Title;
	Price ListingPrice;
	Price AuctionPrice;
	Type ListingType;

	PictureManager PicManager;

signals:
	void AddSelfToUi(Listing*);
public:
	inline Listing(const std::string& title, const std::string& url, const Price& listingPrice, const Type& listingType, const std::vector<std::string>& picVector)
	{
		Title = title;
		URL = url;
		ListingType = listingType;
		switch (ListingType)
		{
		case Type::Normal:
			ListingPrice = listingPrice;
			break;
		case Type::Auction:
			AuctionPrice = listingPrice;
		}

		{ /* Prevent from threads using the same number and double incrementing */
			std::lock_guard<std::mutex> lock(SetIncMutex);
			PicManager.SetStoreDirectory(std::format("{}", DirectoryId));
			DirectoryId++;
		}
		PicManager.AddPicture(picVector);
	}

	void OpenLink()
	{
		QDesktopServices::openUrl(QUrl(QString::fromStdString(URL)));
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

	inline Price GetPrice() const
	{
		switch (ListingType)
		{
		case Type::Normal:
			return ListingPrice;
		case Type::Auction:
			return AuctionPrice;
		}
		throw std::exception("NOT IMPLEMENTED TYPE");
		return ListingPrice;
	}

	inline std::string str()
	{
		return (ListingType == Type::Auction ? "(BID) " : "") + GetPrice().str();
	}

	inline PictureManager* GetPictureManager()
	{
		return &PicManager;
	}
};