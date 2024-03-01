#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

#include "../Headers/PictureManager.hpp"

class PictureDisplay : public QLabel
{
	Q_OBJECT

private:
protected:
	void paintEvent(QPaintEvent* event) override
	{
		QLabel::paintEvent(event);
		//if (!pixmap() || pixmap().isNull())
		//{
		//	return;
		//}

		//QStyle* style = QWidget::style();
		//QPainter painter();
		//drawFrame(&painter);
		//QRect cr = contentsRect();
		//cr.adjust(margin(), margin(), -margin(), -margin());

		//const qreal dpr = devicePixelRatio();
		//QImage image = pixmap().toImage();
		//setPixmap(QPixmap::fromImage(image.scaled(cr.size()* dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
		////style->drawItemPixmap(&painter, cr, align, pix);
	}
public:
	inline PictureDisplay(QWidget* parent = nullptr) : QLabel(parent)
	{
		//By default, this class scales the pixmap according to the label's size
		setScaledContents(true);
	}
};

class PictureCarousel : public QWidget
{
	Q_OBJECT

private:
protected:
	QVBoxLayout* ContainerLayout = nullptr;

	PictureDisplay* Picture = nullptr;

	PictureManager* PicManagerPointer = nullptr;
public:
	inline PictureCarousel(PictureManager* picManagerPointer, QWidget* parent = nullptr) : QWidget(parent)
	{
		PicManagerPointer = picManagerPointer;

		ContainerLayout = new QVBoxLayout(this);
		ContainerLayout->setContentsMargins(0,0,0,0);
		setLayout(ContainerLayout);

		Picture = new PictureDisplay(this);

		if ((*PicManagerPointer)[0] != nullptr)
		{
			Picture->setPixmap(QPixmap::fromImage((*PicManagerPointer)[0]->MakeQImage()));
		}
		ContainerLayout->addWidget(Picture);
	}
};