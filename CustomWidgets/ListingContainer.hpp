#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

#include "Headers/Listing.hpp"
#include "PictureCarousel.hpp"

class ListingContainer : public QWidget
{
	Q_OBJECT

signals:
	inline void MouseReleased(Listing* listingEntry);

protected:
	QVBoxLayout* ContainerLayout = nullptr;
	QLabel* Title = nullptr;

	PictureCarousel* PicCarousel = nullptr;

	Listing* ListingEntry = nullptr;
public:
	inline ListingContainer(Listing* listingEntry, QWidget* parent = nullptr) : QWidget(parent)
	{
		ListingEntry = listingEntry;

		setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		setFixedSize(QSize(250, 250));
		setStyleSheet("background-color:Grey;");

		ContainerLayout = new QVBoxLayout(this);
		ContainerLayout->setContentsMargins(6, 6, 6, 6);
		setLayout(ContainerLayout);

		PicCarousel = new PictureCarousel(listingEntry->GetPictureManager(), this);
		PicCarousel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
		ContainerLayout->addWidget(PicCarousel);

		Title = new QLabel(this);
		Title->setWordWrap(true);
		Title->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));
		Title->setMargin(1);
		Title->setText(QString::fromStdString(ListingEntry->GetTitle()));
		QFont titleFont = Title->font();
		titleFont.setPointSize(12);
		Title->setFont(titleFont);
		ContainerLayout->addWidget(Title);

	}

	~ListingContainer()
	{
		delete ContainerLayout;
		delete Title;
		delete PicCarousel;
		delete ListingEntry;
	}

protected:
	inline void mouseReleaseEvent(QMouseEvent* event) override
	{
		emit MouseReleased(ListingEntry);
	}
};