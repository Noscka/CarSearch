#pragma once

#include <NosLib/DynamicArray.hpp>

#include <vector>
#include <format>
#include <string>

#include "Parser.hpp"

class BaseQuery
{
protected:
	inline static NosLib::DynamicArray<BaseQuery*> QueryClasses;

	virtual NosLib::DynamicArray<std::string> QueryWebsite(const std::string& queryString) = 0;
private:
public:
	BaseQuery() {}

	BaseQuery(const BaseQuery&) = delete;
	BaseQuery(BaseQuery&&) = delete;
	BaseQuery& operator=(const BaseQuery&) = delete;
	BaseQuery& operator=(BaseQuery&&) = delete;

	static void Query(const std::string& queryString, Ui::MainWindow* ui)
	{
		NosLib::DynamicArray<std::string> listings;

		for (BaseQuery* entry : QueryClasses)
		{
			listings += entry->QueryWebsite(queryString);
		}

		for (std::string listingUrl : listings)
		{
			ui->scrollArea->AddNewListingEntry(Parser::ParseWebpage(listingUrl));
		}

		return;
	}
};

class EbayQuery : public BaseQuery
{
private:
	inline static EbayQuery* instance = nullptr;

	inline static std::string QueryURL = ("https://www.ebay.co.uk/sch/i.html?&_nkw={}&_sacat=9801");

	NosLib::DynamicArray<std::string> GetListingArray(html::node* rootNode)
	{
		std::vector<html::node*> foundListings = rootNode->select("a.s-item__link");

		NosLib::DynamicArray<std::string> out;

		for (html::node* entry : foundListings)
		{
			std::string extractedUrl = entry->get_attr("href");

			size_t endPosition = extractedUrl.find_first_of("?");

			extractedUrl = extractedUrl.substr(0, endPosition);

			/* Weird url that always shows up */
			if (extractedUrl == "https://ebay.com/itm/123456")
			{
				continue;
			}

			out.Append(extractedUrl);
		}

		return out;
	}

protected:
	virtual NosLib::DynamicArray<std::string> QueryWebsite(const std::string& queryString) override
	{
		std::string queryRequest = std::vformat(QueryURL, std::make_format_args(queryString));

		NosLib::HostPath urlRequest(queryRequest);

		httplib::Client client = NosLib::MakeClient(urlRequest.Host, false, "CarSearch");

		httplib::Result res = client.Get(urlRequest.Path);

		if (!res)
		{
			fprintf(stderr, "%s\n", httplib::to_string(res.error()).c_str());

			auto result = client.get_openssl_verify_result();
			if (result)
			{
				fprintf(stderr, "verify error: %s\n", X509_verify_cert_error_string(result));
			}

			throw std::invalid_argument(std::format("Failed to connect to {}. Does the website?", urlRequest.Host).c_str());
		}

		std::string content = res->body;

#if 0
		static int fileNum = 1;

		std::ofstream debugFile(std::format("file{}.html", fileNum));
		debugFile.write(content.c_str(), content.size());
		debugFile.close();

		fileNum++;
#endif

		html::parser p;
		html::node_ptr node = p.parse(content);

		return GetListingArray(node.get());
	}
public:
	EbayQuery() {}

	EbayQuery(const EbayQuery&) = delete;
	EbayQuery(EbayQuery&&) = delete;
	EbayQuery& operator=(const EbayQuery&) = delete;
	EbayQuery& operator=(EbayQuery&&) = delete;

	inline static void CreateQueryClass()
	{
		if (instance != nullptr)
		{
			return;
		}

		instance = new EbayQuery();
		QueryClasses.Append(instance);
	}
};


class FacebookQuery : public BaseQuery
{
private:
	inline static FacebookQuery* instance = nullptr;

	inline static std::string Country = "warsaw"; /* Temp */

	inline static std::string QueryURL = ("https://www.facebook.com/marketplace/{}/search/?query={}");

	NosLib::DynamicArray<std::string> GetListingArray(html::node* rootNode)
	{
		using json = nlohmann::json;

		std::vector<html::node*> foundListings = rootNode->select("script[type='application/json']");

		json listingJsonInfo;

		for (html::node* entry : foundListings)
		{
			size_t checkSize = std::string::npos;

			std::string jsonData = entry->to_text();

			checkSize = jsonData.find("marketplace_listing_title");

			if (checkSize == std::string::npos)
			{
				continue;
			}

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

		/* @.require[0][3][0].__bbox.require[0][3][1].__bbox.result.data.marketplace_search.feed_units.edges */
		/* simplify down */
		listingJsonInfo = listingJsonInfo["require"][0][3][0]["__bbox"]["require"][0][3][1]["__bbox"]["result"]["data"]["marketplace_search"]["feed_units"]["edges"];

		NosLib::DynamicArray<std::string> out;

		static std::string marketplaceLink = "https://www.facebook.com/marketplace/item/";

		for (auto& it : listingJsonInfo)
		{
			std::string id = it["node"]["listing"]["id"].get<std::string>();

			out.Append(marketplaceLink + id);
		}

		return out;
	}

protected:
	virtual NosLib::DynamicArray<std::string> QueryWebsite(const std::string& queryString) override
	{
		std::string queryRequest = std::vformat(QueryURL, std::make_format_args(Country, queryString));

		NosLib::HostPath urlRequest(queryRequest);

		httplib::Client client = NosLib::MakeClient(urlRequest.Host, false, "CarSearch");
		client.set_keep_alive(true);
		httplib::Headers headers =
		{
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" }
		};

		httplib::Result res = client.Get(urlRequest.Path, headers);

		if (!res)
		{
			fprintf(stderr, "%s\n", httplib::to_string(res.error()).c_str());

			auto result = client.get_openssl_verify_result();
			if (result)
			{
				fprintf(stderr, "verify error: %s\n", X509_verify_cert_error_string(result));
			}

			throw std::invalid_argument(std::format("Failed to connect to {}. Does the website?", urlRequest.Host).c_str());
		}

		std::string content = res->body;

#if 0
		static int fileNum = 1;

		std::ofstream debugFile(std::format("file{}.html", fileNum));
		debugFile.write(content.c_str(), content.size());
		debugFile.close();

		fileNum++;
#endif

		html::parser p;
		html::node_ptr node = p.parse(content);

		return GetListingArray(node.get());
	}
public:
	FacebookQuery() {}

	FacebookQuery(const FacebookQuery&) = delete;
	FacebookQuery(FacebookQuery&&) = delete;
	FacebookQuery& operator=(const FacebookQuery&) = delete;
	FacebookQuery& operator=(FacebookQuery&&) = delete;

	inline static void CreateQueryClass()
	{
		if (instance != nullptr)
		{
			return;
		}

		instance = new FacebookQuery();
		QueryClasses.Append(instance);
	}
};

inline void InitQueryClasses()
{
	EbayQuery::CreateQueryClass();
	FacebookQuery::CreateQueryClass();
}