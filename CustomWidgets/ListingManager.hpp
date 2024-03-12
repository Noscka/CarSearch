#pragma once
#include <QCoreApplication>
#include <QtWidgets/QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>

#include <NosLib/DynamicArray.hpp>

#include "ListingContainer.hpp"


class LeftToRightLayout : public QGridLayout
{
	Q_OBJECT

private:
	int MaxColumnCount = -1;

public slots:
	void NewAddWidget(QWidget* widget)
	{
		AddWidget(widget);
	}

public:
	LeftToRightLayout(int maxColumnCount, QWidget* parent) : QGridLayout(parent)
	{
		setAlignment(Qt::AlignmentFlag::AlignTop | Qt::AlignmentFlag::AlignLeft);

		MaxColumnCount = maxColumnCount;
	}


	~LeftToRightLayout()
	{

	}

	void AddWidget(QWidget* widget)
	{
		int current_row = 0;
		int current_column = 0;

		while (itemAtPosition(current_row, current_column) != 0)
		{
			if (current_column == (MaxColumnCount - 1))
			{
				current_column = 0;
				++current_row;
			}
			else
			{
				++current_column;
			}
		}

		QGridLayout::addWidget(widget, current_row, current_column);
	}
};

class ListingManager : public QScrollArea
{
	Q_OBJECT



protected:
	static inline NosLib::DynamicArray<ListingContainer*> ListingEntryWidgetArray;

	LeftToRightLayout* ListingLayout;
	QWidget* ListingWidget;

public slots:
	inline void AddNewListingEntry(Listing* newListingEntry)
	{
		AddListingEntry(newListingEntry);
	}

	/*void RemoveListingEntry(Listing* serverEntry)
	{
		for (int i = 0; i <= ListingEntryWidgetArray.GetLastArrayIndex(); i++)
		{
			if (serverEntry != ListingEntryWidgetArray[i]->Server)
			{
				continue;
			}

			this->widget()->layout()->removeWidget(ListingEntryWidgetArray[i]);
			ListingEntryWidgetArray.Remove(i);
			break;
		}
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}*/

public:
	inline ListingManager(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ListingWidget = new QWidget(this);
		setWidget(ListingWidget);

		ListingLayout = new LeftToRightLayout(3, ListingWidget);
		ListingLayout->setContentsMargins(0, 0, 0, 0);
		ListingWidget->setLayout(ListingLayout);

		QCoreApplication::processEvents();
	}

	inline void AddListingEntry(Listing* newListingEntry)
	{
		ListingContainer* newListingEntryContainer = new ListingContainer(newListingEntry, this);
		ListingEntryWidgetArray.Append(newListingEntryContainer);
		LeftToRightLayout* currentLayout = static_cast<LeftToRightLayout*>(this->widget()->layout());
		currentLayout->AddWidget(newListingEntryContainer);

		//this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};