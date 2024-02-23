#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

#include "Headers/Listing.hpp"

class ListingContainer : public QWidget
{
	Q_OBJECT

signals:
	inline void MouseReleased(Listing* listingEntry);

protected:
	QHBoxLayout* ContainerLayout;
	QLabel* Title;
	
	Listing* ListingEntry;
public:
	inline ListingContainer(Listing* listingEntry, QWidget* parent = nullptr) : QWidget(parent)
	{
		setStyleSheet("background-color:Grey;");

		ListingEntry = listingEntry;

		setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		ContainerLayout = new QHBoxLayout();
		ContainerLayout->setContentsMargins(6, 6, 6, 6);
		setLayout(ContainerLayout);

		Title = new QLabel();
		Title->setText(QString::fromStdString(ListingEntry->GetTitle()));
		QFont usernameFont = Title->font();
		usernameFont.setPointSize(16);
		Title->setFont(usernameFont);
		ContainerLayout->addWidget(Title);
	}

protected:
	inline void mouseReleaseEvent(QMouseEvent* event) override
	{
		emit MouseReleased(ListingEntry);
	}
};