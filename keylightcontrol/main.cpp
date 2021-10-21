#include <QApplication>

#include "KeyLightControlWindow.h"

int main(int argv, char** args) {
	QApplication app(argv, args);

	KeyLightControlWindow mainWindow;
	QObject::connect(&mainWindow, SIGNAL(quit()), &app, SLOT(quit()));

	mainWindow.show();

	return app.exec();
}
