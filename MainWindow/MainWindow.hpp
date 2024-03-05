#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

//#define HeaderDebug

#include <NosLib/HttpClient.hpp>
#include <NosLib/HostPath.hpp>

#include "CustomWidgets/ListingContainer.hpp"
#include "CustomWidgets/ListingManager.hpp"
#include "Headers/Parser.hpp"
#include "Headers/Query.hpp"

#include <string>
#include <fstream>
#include <regex>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui;

public:
	inline MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
	{
		ui.setupUi(this);

		BaseQuery::Query("BMW e36", &ui);

		//ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/375266583187"));
		//ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/196258440476"));
		//ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/176262058037"));
		//ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/226008745736?itmmeta=01HQXNXCJ3YM07ZCRCHAEBMJ9K&hash=item349f2bc708:g:9bcAAOSwUJ9lNrvr&itmprp=enc%3AAQAIAAAAwEVSldTD6eqcdCasYRJ--mNowXH%2Bf%2BcJSPrwUyCvYLmm%2FG3l6f795%2BIAUBR7UTW7DnZQpsTUiRZyIw6PfPOaGJyrWRK%2Bk3H8qMDwcqRBQNLXgC1td1LdzS8PRRwIvwd37GhEY%2BXvqH7JxKrC75NQVwfRYxbG1yJbbZ9O%2B9cMGQPEQ5txK2vZpqvHb0142qiqoqk7x3jSdHukeBAekkHlPk8TgA1rjrkVd2lmZ51fWgUrolzniq9BBUnQ8gcuARkSEA%3D%3D%7Ctkp%3ABk9SR47J9bW_Yw"));
	}

	inline ~MainWindow()
	{

	}
};
