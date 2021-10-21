#include <QApplication>

#include "KeyLightControlWindow.h"

int main(int argv, char** args) {
	QApplication app(argv, args);

	KeyLightControlWindow mainWindow;

	mainWindow.show();

	return app.exec();
}
