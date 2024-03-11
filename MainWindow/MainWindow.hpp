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

		//BaseQuery::Query("BMW e36", &ui);

		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/375305077858")); /* Not Auction */
		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/335296325051")); /* Auction with "Buy Now" Price */ 
		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/126370708923")); /* Auction without "Buy Now" Price */
		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.pl/itm/394564084936")); /* Polish */
	}

	inline ~MainWindow()
	{

	}
};
