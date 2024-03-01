#pragma once

#include <NosLib/HostPath.hpp>
#include <NosLib/DynamicArray.hpp>

#include <html.hpp>
#include <nlohmann/Json.hpp>

#include "Listing.hpp"
#include "PictureManager.hpp"

#include <string>

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
			throw std::invalid_argument(std::format("Failed to find parser for {}. Website not supported", url.Host).c_str());
			return nullptr;
		}

		httplib::Client client = NosLib::MakeClient(url.Host, false, "CarSearch");

		httplib::Result res = client.Get(url.Path);

		if (!res)
		{
			fprintf(stderr, "%s\n", httplib::to_string(res.error()).c_str());

			auto result = client.get_openssl_verify_result();
			if (result)
			{
				fprintf(stderr, "verify error: %s\n", X509_verify_cert_error_string(result));
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

	inline std::string GetEbayTitle(const nlohmann::json& listingJsonInfo)
	{
		std::string title = listingJsonInfo["TITLE"]["mainTitle"]["textSpans"][0]["text"].get<std::string>();

		return title;
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
		return (url.Host == "https://www.ebay.co.uk");
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

		{
			html::parser p;
			html::node_ptr node = p.parse(content);

			std::vector<html::node*> jsonScriptNode = node->select("html>body>script:last");

			std::string jsonData = jsonScriptNode[0]->to_text();

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

		return new Listing(GetEbayTitle(listingJsonInfo), url.Full(), GetPictureLinks(listingJsonInfo));
	}
};

inline void InitParsers()
{
	EbayParser::GetParser();
}