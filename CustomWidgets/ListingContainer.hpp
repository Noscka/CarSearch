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
	QLabel* Title = nullptr;
	QLabel* PriceLabel = nullptr;

	PictureCarousel* PicCarousel = nullptr;

	Listing* ListingEntry = nullptr;

public:
	inline ListingContainer(Listing* listingEntry, QWidget* parent = nullptr) : QWidget(parent)
	{
		ListingEntry = listingEntry;

		setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		setFixedSize(QSize(350, 350));

		ContainerLayout = new QVBoxLayout(this);
		ContainerLayout->setContentsMargins(6, 6, 6, 6);
		ContainerLayout->setSpacing(0);
		setLayout(ContainerLayout);

		PicCarousel = new PictureCarousel(listingEntry->GetPictureManager(), this);
		PicCarousel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
		ContainerLayout->addWidget(PicCarousel);

		PriceLabel = new QLabel(this);
		PriceLabel->setWordWrap(false);
		PriceLabel->setText(QString::fromStdString(ListingEntry->str()));
		QFont priceFont = PriceLabel->font();
		priceFont.setPointSize(12);
		priceFont.setBold(true);
		PriceLabel->setFont(priceFont);
		PriceLabel->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
		ContainerLayout->addWidget(PriceLabel);

		Title = new QLabel(this);
		Title->setWordWrap(true);
		Title->setMargin(1);
		Title->setText(QString::fromStdString(ListingEntry->GetTitle()));
		QFont titleFont = Title->font();
		titleFont.setPointSize(10);
		Title->setFont(titleFont);
		PriceLabel->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
		ContainerLayout->addWidget(Title);

		QObject::connect(this, &ListingContainer::MouseReleased, [&ListingEntry = ListingEntry]() { ListingEntry->OpenLink(); });
	}

	~ListingContainer()
	{
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