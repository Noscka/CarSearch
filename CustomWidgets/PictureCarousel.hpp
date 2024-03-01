#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

class PictureCarousel : public QWidget
{
	Q_OBJECT

private:
protected:
public:
	inline PictureCarousel(Listing* listingEntry, QWidget* parent = nullptr) : QWidget(parent)
	{

	}
};