#pragma once

#include <QMainWindow>
#include <QtGui>

class KeyLightControlWindow : public QWidget {
	Q_OBJECT

public:
	KeyLightControlWindow();

private slots:
	void turnOn();
	void turnOff();
	void getStatus();

private:
};