#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

//#define HeaderDebug

#include <NosLib/HttpClient.hpp>
#include <NosLib/HostPath.hpp>

#include "CustomWidgets/ListingContainer.hpp"
#include "CustomWidgets/ListingManager.hpp"
#include "Headers/Parser.hpp"

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
		InnitParsers();
		ui.setupUi(this);

		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/375266583187"));
		ui.scrollArea->AddNewListingEntry(Parser::ParseWebpage("https://www.ebay.co.uk/itm/196258440476"));
		ui.scrollArea->AddNewListingEntry(new Listing("1", "a", "a", {}));
		ui.scrollArea->AddNewListingEntry(new Listing("2", "a", "a", {}));
		ui.scrollArea->AddNewListingEntry(new Listing("3", "a", "a", {}));
		ui.scrollArea->AddNewListingEntry(new Listing("4", "a", "a", {}));
	}

	inline ~MainWindow()
	{

	}
};
