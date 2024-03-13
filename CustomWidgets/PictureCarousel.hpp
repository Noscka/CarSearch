#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QtWidgets/QPushButton>
#include <QSpacerItem>
#include <QWidget>

#include "../Headers/PictureManager.hpp"

class PictureDisplay : public QLabel
{
	Q_OBJECT

private:
	QPixmap pix;
public slots:
	void setPixmap(const QPixmap& p)
	{
		pix = p;
		QLabel::setPixmap(scaledPixmap());
	}

	void resizeEvent(QResizeEvent* event) override
	{
		QLabel::resizeEvent(event);

		if (!pix.isNull())
			QLabel::setPixmap(scaledPixmap());
	}
public:
	inline PictureDisplay(QWidget* parent = nullptr) : QLabel(parent)
	{
		//By default, this class scales the pixmap according to the label's size
		setScaledContents(false);
	}

	QPixmap scaledPixmap() const
	{
		//return pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		return pix.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
};

class PictureCarousel : public QWidget
{
	Q_OBJECT

private:
protected:
	QHBoxLayout* ContainerLayout = nullptr;

	PictureDisplay* Picture = nullptr;
	QHBoxLayout* OnPictureLayout = nullptr;

	QPushButton* PreviousPictureButton = nullptr;
	QPushButton* NextPictureButton = nullptr;

	PictureManager* PicManagerPointer = nullptr;

	void UpdatePicture()
	{
		Picture->setPixmap(PicManagerPointer->GetPixmap());
		QCoreApplication::processEvents();
	}
public:
	inline PictureCarousel(PictureManager* picManagerPointer, QWidget* parent = nullptr) : QWidget(parent)
	{
		PicManagerPointer = picManagerPointer;

		ContainerLayout = new QHBoxLayout(this);
		ContainerLayout->setContentsMargins(0,0,0,0);
		setLayout(ContainerLayout);

		Picture = new PictureDisplay(this);
		OnPictureLayout = new QHBoxLayout(Picture);
		Picture->setLayout(OnPictureLayout);

		PreviousPictureButton = new QPushButton(this);
		PreviousPictureButton->setContentsMargins(0,0,0,0);
		PreviousPictureButton->setIcon(QIcon(":/Resources/Icons/LeftArrow.png"));
		OnPictureLayout->setContentsMargins(0, 0, 0, 0);
		OnPictureLayout->addWidget(PreviousPictureButton);
		OnPictureLayout->addStretch();

		Picture->setPixmap(PicManagerPointer->GetPixmap());
		ContainerLayout->addWidget(Picture, 0, Qt::AlignCenter);

		NextPictureButton = new QPushButton(this);
		NextPictureButton->setContentsMargins(0, 0, 0, 0);
		NextPictureButton->setIcon(QIcon(":/Resources/Icons/RightArrow.png"));
		OnPictureLayout->addWidget(NextPictureButton);

		connect(PreviousPictureButton, &QPushButton::released, [this]() { PicManagerPointer->PreviousImage(); UpdatePicture(); });
		connect(NextPictureButton, &QPushButton::released, [this]() { PicManagerPointer->NextImage(); UpdatePicture(); });
	}

	~PictureCarousel()
	{
		delete ContainerLayout;
		delete OnPictureLayout;
		delete PreviousPictureButton;
		delete NextPictureButton;
		delete Picture;
	}
};