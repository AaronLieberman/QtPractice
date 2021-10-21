#include <QApplication>

#include "KeyLightControlWindow.h"

int main(int argv, char** args) {
	QApplication app(argv, args);

	KeyLightControlWindow mainWindow("192.168.87.26", "9123");

	mainWindow.show();

	return app.exec();
}
