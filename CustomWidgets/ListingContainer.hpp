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
	QVBoxLayout* ContainerLayout;
	QLabel* Picture;
	QLabel* Title;
	QLabel* Description;
	
	Listing* ListingEntry;
public:
	inline ListingContainer(Listing* listingEntry, QWidget* parent = nullptr) : QWidget(parent)
	{

		ListingEntry = listingEntry;

		setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
		ContainerLayout = new QVBoxLayout();
		ContainerLayout->setContentsMargins(6, 6, 6, 6);
		setLayout(ContainerLayout);

		ContainerLayout->setStyleSheet("background-color:Grey;");

		Picture = new QLabel();
		QImage imagePixmap;
		imagePixmap.load(R"(C:\Users\Adam\Downloads\CarDescription.png)");
		Picture->setPixmap(QPixmap::fromImage(imagePixmap.scaled(QSize(120,120), Qt::KeepAspectRatio, Qt::FastTransformation)));
		ContainerLayout->addWidget(Picture);

		Title = new QLabel();
		Title->setText(QString::fromStdString(ListingEntry->GetTitle()));
		QFont titleFont = Title->font();
		titleFont.setPointSize(12);
		Title->setFont(titleFont);
		ContainerLayout->addWidget(Title);

		Description = new QLabel();
		Description->setText(QString::fromStdString(ListingEntry->GetDescription()));
		QFont descriptionFont = Description->font();
		descriptionFont.setPointSize(10);
		Description->setFont(descriptionFont);
		ContainerLayout->addWidget(Description);
	}

protected:
	inline void mouseReleaseEvent(QMouseEvent* event) override
	{
		emit MouseReleased(ListingEntry);
	}
};