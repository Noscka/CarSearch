#pragma once
#include <QCoreApplication>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>

#include <NosLib/DynamicArray.hpp>

#include "FlowLayout.hpp"
#include "ListingContainer.hpp"

class ListingManager : public QScrollArea
{
	Q_OBJECT

protected:
	static inline NosLib::DynamicArray<ListingContainer*> ListingEntryWidgetArray;

	FlowLayout* ListingLayout;
	QWidget* ListingWidget;

public slots:
	inline void AddNewListingEntry(Listing* newListingEntry)
	{
		AddListingEntry(newListingEntry);
	}

	void Clear()
	{ 
		for (ListingContainer* entry : ListingEntryWidgetArray)
		{
			delete entry;
		}

		ListingEntryWidgetArray.Clear();
		QCoreApplication::processEvents();
	}

public:
	inline ListingManager(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ListingWidget = new QWidget(this);
		setWidget(ListingWidget);

		ListingLayout = new FlowLayout(ListingWidget);
		ListingLayout->setContentsMargins(0, 0, 0, 0);
		ListingWidget->setLayout(ListingLayout);

		QCoreApplication::processEvents();
	}

	inline void AddListingEntry(Listing* newListingEntry)
	{
		ListingContainer* newListingEntryContainer = new ListingContainer(newListingEntry, this);
		ListingEntryWidgetArray.Append(newListingEntryContainer);
		QLayout* currentLayout = this->widget()->layout();
		currentLayout->addWidget(newListingEntryContainer);

		//this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};