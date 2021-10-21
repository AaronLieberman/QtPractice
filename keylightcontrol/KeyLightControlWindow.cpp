#include <QApplication>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGui>

#include "KeyLightControlWindow.h"

KeyLightControlWindow::KeyLightControlWindow() {
	QTextEdit* textEdit = new QTextEdit();
	QPushButton* quitButton = new QPushButton("&Quit");

	QObject::connect(quitButton, SIGNAL(clicked()), this, SIGNAL(quit()));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(textEdit);
	layout->addWidget(quitButton);

	setLayout(layout);
}

void KeyLightControlWindow::turnOn() {}
void KeyLightControlWindow::turnOff() {}
void KeyLightControlWindow::getStatus() {}
