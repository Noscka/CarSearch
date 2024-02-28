#pragma once

#include <NosLib/HostPath.hpp>
#include <NosLib/DynamicArray.hpp>

#include <html.hpp>

#include "Listing.hpp"

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

		httplib::Client client = NosLib::MakeClient(url.Host, "CarSearch");

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

#if 1
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

	inline std::string ConvertToNormal(std::string content)
	{
		std::string out;
		std::smatch matches;

		while (std::regex_search(content, matches, std::regex("&#([0-9]+);")))
		{
			std::string currentMatch = matches[0].str();

			uint8_t numStart = 2;
			uint8_t numLenght = currentMatch.length() - (numStart + 1);

			std::string charNumStr = currentMatch.substr(numStart, numLenght);

			int charNum = std::stoi(charNumStr);

			out.push_back((char)charNum);

			content = matches.suffix().str();
		}

		return out;
	}

	inline std::string GetEbayTitle(html::node* htmlRootNode)
	{
		std::string titleData = htmlRootNode->select("div.vi-title__main")[0]->to_text();

		titleData = std::regex_replace(titleData, std::regex("&#34;"), "\"");
		titleData = std::regex_replace(titleData, std::regex("&quot;"), "\"");
		titleData = std::regex_replace(titleData, std::regex("&amp;"), "&");
		titleData = std::regex_replace(titleData, std::regex("&lt;"), "<");
		titleData = std::regex_replace(titleData, std::regex("&gt;"), ">");

		titleData = ConvertToNormal(titleData);
		return titleData;
	}

	inline std::vector<std::string> GetPictureLinks(html::node* htmlRootNode)
	{
		/* o->w->5->3->model->PICTURE->mediaList->[image/zoomImg] */
		std::vector<html::node*> foundImageNodes = htmlRootNode->select("div.image-container");

		for (html::node* imgEntries : foundImageNodes)
		{
			std::cout << imgEntries->to_raw_html() << std::endl;
			//printf("%s\n", imgEntries->get_attr(""))
		}

		std::vector<std::string> out;

		return out;
	}

	inline std::string GetEbayDescription(html::node* htmlRootNode)
	{
		throw std::exception("Doesn't work");

#if 0
		const static std::string startTag = "<div class=\"ux-layout-section__textual-display ux-layout-section__textual-display--shortDescription\"><span class=\"ux-textspans\">";
		const static std::string endTag = "</span></div>";

		std::ofstream test("abc.html");
		test.write(htmlContent.c_str(), htmlContent.size());
		test.close();

		std::stringstream htmlStream(htmlContent);

		std::string titleData;
		std::string line;

		size_t linePosition = 0;
		size_t startPosition = std::string::npos;
		size_t endPosition = std::string::npos;

		while (std::getline(htmlStream, line))
		{
			if (startPosition == std::string::npos)
			{
				startPosition = line.find(startTag);

				if (startPosition == std::string::npos)
				{
					linePosition += line.size();
					continue;
				}

				startPosition += linePosition + startTag.size();
			}

			endPosition = line.find(endTag, startPosition);

			if (endPosition == std::string::npos)
			{
				linePosition += line.size();
				continue;
			}

			endPosition += linePosition;

			break;
		}

		std::string description = htmlStream.str().substr(startPosition, endPosition - startPosition);

		return description;
#endif
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
		html::parser p;
		html::node_ptr node = p.parse(content);

		(void)GetPictureLinks(node.get());

		return new Listing(GetEbayTitle(node.get()), "", url.Full());
	}
};

inline void InnitParsers()
{
	EbayParser::GetParser();
}