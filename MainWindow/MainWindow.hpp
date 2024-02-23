#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "CustomWidgets/ListingContainer.hpp"
#include "CustomWidgets/ListingManager.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui;

public:
	inline MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
	{
		ui.setupUi(this);

		ui.scrollArea->AddNewListingEntry(new Listing("1"));
		ui.scrollArea->AddNewListingEntry(new Listing("2"));
		ui.scrollArea->AddNewListingEntry(new Listing("3"));
		ui.scrollArea->AddNewListingEntry(new Listing("4"));
	}

	inline ~MainWindow()
	{

	}
};
