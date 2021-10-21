#pragma once

#include <QMainWindow>
#include <QtGui>

class KeyLightControlWindow : public QWidget {
	Q_OBJECT

public:
	KeyLightControlWindow();

signals:
	void quit();

private slots:
	void turnOn();
	void turnOff();
	void getStatus();

private:
};