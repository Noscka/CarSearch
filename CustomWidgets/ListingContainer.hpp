#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLabel>
#include <QtWidgets\QLayout>
#include <QWidget>

#include "Headers/Listing.hpp"
#include "PictureCarousel.hpp"

class ListingContainer : public QWidget
{
	Q_OBJECT

signals:
	inline void MouseReleased();

protected:
	QVBoxLayout* ContainerLayout = nullptr;
	QHBoxLayout* LowerInfoContainerLayout = nullptr;
	QLabel* Title = nullptr;
	QLabel* PriceLabel = nullptr;

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

		LowerInfoContainerLayout = new QHBoxLayout();
		LowerInfoContainerLayout->setContentsMargins(0,0,0,0);
		ContainerLayout->addLayout(LowerInfoContainerLayout);

		Title = new QLabel(this);
		Title->setWordWrap(true);
		Title->setMargin(1);
		Title->setText(QString::fromStdString(ListingEntry->GetTitle()));
		QFont titleFont = Title->font();
		titleFont.setPointSize(12);
		Title->setFont(titleFont);
		LowerInfoContainerLayout->addWidget(Title);

		PriceLabel = new QLabel(this);
		PriceLabel->setWordWrap(false);
		PriceLabel->setText(QString::fromStdString(ListingEntry->str()));
		QFont priceFont = PriceLabel->font();
		priceFont.setPointSize(12);
		PriceLabel->setFont(priceFont);
		LowerInfoContainerLayout->addWidget(PriceLabel);

		QObject::connect(this, &ListingContainer::MouseReleased, [&ListingEntry = ListingEntry]() { ListingEntry->OpenLink(); });
	}

	~ListingContainer()
	{
		delete LowerInfoContainerLayout;
		delete ContainerLayout;
		delete Title;
		delete PriceLabel;
		delete PicCarousel;
		delete ListingEntry;
	}

protected:
	inline void mouseReleaseEvent(QMouseEvent* event) override
	{
		emit MouseReleased();
	}
};