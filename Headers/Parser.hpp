#pragma once

#include <NosLib/HostPath.hpp>
#include <NosLib/DynamicArray.hpp>
#include <NosLib/Logging.hpp>

#include <html.hpp>
#include <nlohmann/Json.hpp>

#include "Listing.hpp"
#include "PictureManager.hpp"

#include <string>
#include <tuple>

class Parser
{
protected:
	inline static NosLib::DynamicArray<Parser*> Parsers;

	inline virtual bool Check(const NosLib::HostPath& url) = 0;

	inline static Parser* DetermineParser(const NosLib::HostPath& url)
	{
		for (Parser* entry : Parsers)
		{
			if (entry->Check(url))
			{
				return entry;
			}
		}

		return nullptr;
	}

	inline static std::mutex ConnectionMutex;
public:
	Parser() {}

	Parser(const Parser&) = delete;
	Parser(Parser&&) = delete;
	Parser& operator=(const Parser&) = delete;
	Parser& operator=(Parser&&) = delete;

	virtual Listing* Parse(const std::string& content, const NosLib::HostPath& url) = 0;

	inline static Listing* ParseWebpage(const NosLib::HostPath& url)
	{
		Parser* parser = Parser::DetermineParser(url);

		if (parser == nullptr)
		{
			NosLib::Logging::CreateLog<char>(std::format("Failed to find parser for {}. Website not supported", url.Host), NosLib::Logging::Severity::Error);
			throw std::invalid_argument(std::format("Failed to find parser for {}. Website not supported", url.Host).c_str());
			return nullptr;
		}

		httplib::Client client = NosLib::MakeClient(url.Host, false, "CarSearch");

		httplib::Headers headers =
		{
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" }
		};

		httplib::Result res = client.Get(url.Path, headers);

		if (!res)
		{
			NosLib::Logging::CreateLog<char>(httplib::to_string(res.error()), NosLib::Logging::Severity::Error);

			auto result = client.get_openssl_verify_result();
			if (result)
			{
				NosLib::Logging::CreateLog<char>(std::format("OpenSSL Error: {}\n", X509_verify_cert_error_string(result)), NosLib::Logging::Severity::Error);
			}

			throw std::invalid_argument(std::format("Failed to connect to {}. Does the website?", url.Host).c_str());
			return nullptr;
		}

		std::string content = res->body;

#if 0
		static int fileNum = 1;

		std::ofstream debugFile(std::format("file{}.html", fileNum));
		debugFile.write(content.c_str(), content.size());
		debugFile.close();

		fileNum++;
#endif

		return parser->Parse(content, url);
	}

	inline static Listing* ParseWebpage(const std::string& url)
	{
		return ParseWebpage(NosLib::HostPath(url));
	}
};

class EbayParser : public Parser
{
private:
	inline static EbayParser* instance = nullptr;

	inline std::string GetTitle(const nlohmann::json& listingJsonInfo)
	{
		std::string title = listingJsonInfo["TITLE"]["mainTitle"]["textSpans"][0]["text"].get<std::string>();

		return title;
	}

	inline Price::Currency GetCurrency(const nlohmann::json& listingJsonInfo)
	{
		std::string currencyString = listingJsonInfo["currency"].get<std::string>();

		if (currencyString == "GBP")
		{
			return Price::Currency::GBP;
		}
		else if (currencyString == "PLN")
		{
			return Price::Currency::ZLOTY;
		}

		throw std::exception("NOT IMPLEMENTED TYPE");
		return Price::Currency::ZLOTY;
	}

	inline std::tuple<Price, Listing::Type> GetPricing(const nlohmann::json& listingJsonInfo)
	{
		Listing::Type type = Listing::Type::Normal;

		if (listingJsonInfo["SEMANTIC_DATA"].contains("auctionPossible"))
		{
			type = Listing::Type::Auction;
		}

		nlohmann::json tempOffset;

		if (listingJsonInfo["BUY_BOX"].contains("binModel")) /* if has buy now price, use that for now */
		{
			tempOffset = listingJsonInfo["BUY_BOX"]["binModel"]["price"]["value"];
		}
		else if(listingJsonInfo["BUY_BOX"].contains("bidPrice"))
		{
			tempOffset = listingJsonInfo["BUY_BOX"]["bidPrice"]["value"];
		}
		else if(listingJsonInfo["BUY_BOX"].contains("offerPrice"))
		{
			tempOffset = listingJsonInfo["BUY_BOX"]["offerPrice"]["value"];
		}
		else
		{
			throw std::exception("Cannot find where price is located");
		}

		uint64_t value = tempOffset["value"].get<uint64_t>();
		Price::Currency currency = GetCurrency(tempOffset);

		return std::make_tuple(Price(value, currency), type);

	}

	inline std::vector<std::string> GetPictureLinks(const nlohmann::json& listingJsonInfo)
	{
		std::vector<std::string> out;

		for (auto& it : listingJsonInfo["PICTURE"]["mediaList"])
		{
			std::string url = it["image"]["originalImg"]["URL"].get<std::string>();
			out.emplace_back(url);
		}

		return out;
	}

protected:
	inline virtual bool Check(const NosLib::HostPath& url) override
	{
		return (url.Host.find("https://www.ebay") != std::string::npos); /* if host contains ebay, so it works with global websites */
	}

public:
	EbayParser() {}

	EbayParser(const EbayParser&) = delete;
	EbayParser(EbayParser&&) = delete;
	EbayParser& operator=(const EbayParser&) = delete;
	EbayParser& operator=(EbayParser&&) = delete;

	inline static EbayParser* GetParser()
	{
		if (instance == nullptr)
		{
			instance = new EbayParser();
			Parsers.Append(instance);
		}

		return instance;
	}

	virtual Listing* Parse(const std::string& content, const NosLib::HostPath& url)
	{
		using json = nlohmann::json;

		json listingJsonInfo;
		std::string jsonData; /* DEBUG to see string */

		{
			html::parser p;
			html::node_ptr node = p.parse(content);

			std::vector<html::node*> jsonScriptNode = node->select("html>body>script:last");

			jsonData = jsonScriptNode[0]->to_text();

			const static std::string startTag = ".concat(";
			const static std::string endTag = ")";

			size_t startPosition = std::string::npos;
			size_t endPosition = std::string::npos;

			startPosition = jsonData.find(startTag);

			if (startPosition == std::string::npos)
			{
				fprintf(stderr, "Line %d in function %s: %s\n", __LINE__, __FUNCTION__, "Failed to find json data");
				return nullptr;
			}
			startPosition += startTag.size();

			endPosition = jsonData.find_last_of(endTag);

			jsonData = jsonData.substr(startPosition, endPosition - startPosition);

#if 0
			static int fileNum = 1;

			std::ofstream debugFile(std::format("ebay{}.json", fileNum));
			debugFile.write(jsonData.c_str(), jsonData.size());
			debugFile.close();

			fileNum++;
#endif

			try
			{
				listingJsonInfo = json::parse(jsonData);
			}
			catch (const std::exception& ex)
			{
				fprintf(stderr, "Line %d in function %s: %s\n", __LINE__, __FUNCTION__, ex.what());
				return nullptr;
			}
			//printf("%s\n", jsonData.c_str());
		}

		/* offset to useful data position */
		listingJsonInfo = listingJsonInfo["o"]["w"][4][2]["model"];

		if (listingJsonInfo.contains("placementId"))
		{
			throw std::exception("Invalid json data");
		}

		std::string listingTitle = GetTitle(listingJsonInfo);
		const auto[listingPrice, listingType] = GetPricing(listingJsonInfo);
		std::vector<std::string> listingPictures = GetPictureLinks(listingJsonInfo);

		/* BidInfo in GetPricing gives bid count and etc */
		return new Listing(listingTitle, url.Full(), listingPrice, listingType, listingPictures);
	}
};

class FacebookParser : public Parser
{
private:
	inline static FacebookParser* instance = nullptr;

	inline std::string GetTitle(const nlohmann::json& listingJsonInfo)
	{
		/* .marketplace_listing_title */

		std::string title = listingJsonInfo["marketplace_listing_title"].get<std::string>();

		return title;
	}

	inline Price::Currency GetCurrency(const nlohmann::json& listingJsonInfo)
	{
		std::string currencyString = listingJsonInfo["currency"].get<std::string>();

		if (currencyString == "GBP")
		{
			return Price::Currency::GBP;
		}
		else if (currencyString == "PLN")
		{
			return Price::Currency::ZLOTY;
		}
		else if (currencyString == "USD")
		{
			return Price::Currency::USD;
		}

		throw std::exception("NOT IMPLEMENTED TYPE");
		return Price::Currency::ZLOTY;
	}

	inline Price GetPricing(const nlohmann::json& listingJsonInfo)
	{
		nlohmann::json tempOffset = listingJsonInfo["listing_price"];

		uint64_t value = std::stoull(tempOffset["amount"].get<std::string>());
		Price::Currency currency = GetCurrency(tempOffset);

		return Price(value, currency);
	}

	inline std::vector<std::string> GetPictureLinks(const nlohmann::json& listingJsonInfo)
	{
		/* .listing_photos */

		std::vector<std::string> out;

		for (auto& it : listingJsonInfo["listing_photos"])
		{
			std::string url = it["image"]["uri"].get<std::string>();
			out.emplace_back(url);
		}

		return out;
	}

protected:
	inline virtual bool Check(const NosLib::HostPath& url) override
	{
		return (url.Host == "https://www.facebook.com");
	}

public:
	FacebookParser() {}

	FacebookParser(const FacebookParser&) = delete;
	FacebookParser(FacebookParser&&) = delete;
	FacebookParser& operator=(const FacebookParser&) = delete;
	FacebookParser& operator=(FacebookParser&&) = delete;

	inline static FacebookParser* GetParser()
	{
		if (instance == nullptr)
		{
			instance = new FacebookParser();
			Parsers.Append(instance);
		}

		return instance;
	}

	virtual Listing* Parse(const std::string& content, const NosLib::HostPath& url)
	{
		using json = nlohmann::json;

		json listingJsonInfo;

		{
			html::parser p;
			html::node_ptr node = p.parse(content);

			std::vector<html::node*> foundScripts = node->select("script[type='application/json']");

			for (html::node* entry : foundScripts)
			{
				size_t checkSize = std::string::npos;

				std::string jsonData = entry->to_text();

				checkSize = jsonData.find("marketplace_product_details_page"); /* Some unique string */

				if (checkSize == std::string::npos)
				{
					continue;
				}

#if 0
				static int fileNum = 1;

				std::ofstream debugFile(std::format("facebook{}.json", fileNum));
				debugFile.write(jsonData.c_str(), jsonData.size());
				debugFile.close();

				fileNum++;
#endif

				try
				{
					listingJsonInfo = json::parse(jsonData);
					break;
				}
				catch (const std::exception& ex)
				{
					fprintf(stderr, "Line %d in function %s: %s\n", __LINE__, __FUNCTION__, ex.what());
					throw ex;
				}
			}
		}

		/* @.require[0][3][0].__bbox.require[3][3][1].__bbox.result.data.viewer.marketplace_product_details_page.target */
		/* offset to useful data position */
		listingJsonInfo = listingJsonInfo["require"][0][3][0]["__bbox"]["require"][3][3][1]["__bbox"]["result"]["data"]["viewer"]["marketplace_product_details_page"]["target"];

		return new Listing(GetTitle(listingJsonInfo), url.Full(), GetPricing(listingJsonInfo), Listing::Type::Normal, GetPictureLinks(listingJsonInfo));
	}
};

inline void InitParsers()
{
	EbayParser::GetParser();
	FacebookParser::GetParser();
}