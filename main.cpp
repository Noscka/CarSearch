#include <QtWidgets/QApplication>

#include "MainWindow/MainWindow.hpp"

#include "Headers/Parser.hpp"
#include "Headers/Query.hpp"

int main(int argc, char* argv[])
{
	InitParsers();
	InitQueryClasses();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}