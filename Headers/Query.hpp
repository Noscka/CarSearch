#pragma once

#include <NosLib/DynamicArray.hpp>

#include <vector>
#include <format>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

#include "Parser.hpp"
#include "ThreadPool.hpp"

inline static void ThreadedParsing(NosLib::DynamicArray<WorkHolder<std::string>>* workItems, Ui::MainWindow* ui)
{
	for (WorkHolder<std::string>& listingEntry : (*workItems))
	{
		if (listingEntry.GetWorkStatus() != WorkStatus::Unfinished) /* if already started or finished, continue */
		{
			continue;
		}

		listingEntry.SetWorkStatus(WorkStatus::Started);

		try
		{
			Listing* newListing = Parser::ParseWebpage(listingEntry.GetWorkItem());
			QObject::connect(newListing, &Listing::AddSelfToUi, ui->scrollArea, &ListingManager::AddNewListingEntry);
			newListing->AddSelfToUiFunc();
		}
		catch (const std::exception ex)
		{
			fprintf(stderr, "%s\nListing %s skipped\n", ex.what(), listingEntry.GetWorkItem().c_str());
		}

		listingEntry.SetWorkStatus(WorkStatus::Finished);
	}
}

class BaseQuery
{
protected:
	inline static NosLib::DynamicArray<BaseQuery*> QueryClasses;

	std::mutex ArrayMutex;
	std::mutex CreateClientMutex;

	virtual void QueryWebsite(const std::string& queryString, NosLib::DynamicArray<std::string>* outArray) = 0;
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

		{
			NosLib::DynamicArray<std::thread*> queryThreads;

			for (BaseQuery* entry : QueryClasses)
			{
				queryThreads.Append(new std::thread(&BaseQuery::QueryWebsite, entry, queryString, &listings));
				Sleep(10);
			}

			for (std::thread* entry : queryThreads)
			{
				entry->join();
			}
		}

		DeviceDependentThreadPool<std::string>::StartThreadPool(&ThreadedParsing, listings, ui);
	}
};

class EbayQuery : public BaseQuery
{
private:
	inline static EbayQuery* instance = nullptr;

	inline static std::string QueryURL = ("https://www.ebay.co.uk/sch/i.html?&_nkw={}&_sacat=9801");

	void ExtractListingArray(html::node* rootNode, NosLib::DynamicArray<std::string>* outArray)
	{
		std::vector<html::node*> foundListings = rootNode->select("a.s-item__link");

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

			std::lock_guard<std::mutex> lock(ArrayMutex);
			outArray->Append(extractedUrl);
		}
	}

protected:
	virtual void QueryWebsite(const std::string& queryString, NosLib::DynamicArray<std::string>* outArray) override
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

		ExtractListingArray(node.get(), outArray);
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

	void ExtractListingArray(html::node* rootNode, NosLib::DynamicArray<std::string>* outArray)
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

		static std::string marketplaceLink = "https://www.facebook.com/marketplace/item/";

		for (auto& it : listingJsonInfo)
		{
			std::string id = it["node"]["listing"]["id"].get<std::string>();

			std::lock_guard<std::mutex> lock(ArrayMutex);
			outArray->Append(marketplaceLink + id);
		}
	}
protected:
	virtual void QueryWebsite(const std::string& queryString, NosLib::DynamicArray<std::string>* outArray) override
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

		ExtractListingArray(node.get(), outArray);
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