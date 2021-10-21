#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGui>

#include "KeyLightControlWindow.h"

template <typename TParent, typename TLayout>
TParent* applyLayout(TParent* parent, std::initializer_list<QWidget*> widgets) {
	QLayout* layout = new TLayout();

	for (QWidget* widget : widgets) {
		layout->addWidget(widget);
	}

	parent->setLayout(layout);
	return parent;
}

template <typename TParent, typename TLayout>
TParent* applyLayout(std::initializer_list<QWidget*> widgets) {
	return applyLayout<TParent, TLayout>(new TParent(), std::move(widgets));
}

KeyLightControlWindow::KeyLightControlWindow() {
	QLabel* statusLabel = new QLabel("Status");
	QFrame* statusFrame = applyLayout<QFrame, QVBoxLayout>({statusLabel});

	QPushButton* onButton = new QPushButton("&On");
	QObject::connect(onButton, SIGNAL(clicked()), this, SIGNAL(turnOn()));
	QPushButton* offButton = new QPushButton("&Off");
	QObject::connect(offButton, SIGNAL(clicked()), this, SIGNAL(turnOff()));

	QFrame* buttonFrame = applyLayout<QFrame, QHBoxLayout>({onButton, offButton});

	applyLayout<QWidget, QVBoxLayout>(this, {statusFrame, buttonFrame});
}

void KeyLightControlWindow::turnOn() {
	// TODO
}

void KeyLightControlWindow::turnOff() {
	// TODO
}

void KeyLightControlWindow::getStatus() {
	// TODO
}
