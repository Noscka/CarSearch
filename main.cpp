#define WIN32_LEAN_AND_MEAN

#include <QtWidgets/QApplication>

#include "MainWindow/MainWindow.hpp"

#include <NosLib/Logging.hpp>

#include "Headers/Parser.hpp"
#include "Headers/Query.hpp"

int main(int argc, char* argv[])
{
	NosLib::Logging::SetVerboseLevel(NosLib::Logging::Verbose::Debug);

	InitParsers();
	InitQueryClasses();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}