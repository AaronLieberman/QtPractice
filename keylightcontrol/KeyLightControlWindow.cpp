#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGui>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <string>

#include "KeyLightControlWindow.h"

namespace {
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
} // namespace

KeyLightControlWindow::KeyLightControlWindow()
    : _networkManager(std::make_unique<QNetworkAccessManager>()) {
	QLabel* statusLabel = new QLabel("LightStatus");
	_statusFrame = applyLayout<QFrame, QVBoxLayout>({});
	QFrame* statusGroupFrame = applyLayout<QFrame, QVBoxLayout>({statusLabel, _statusFrame});

	QPushButton* onButton = new QPushButton("&On");
	QObject::connect(onButton, SIGNAL(clicked()), this, SLOT(turnOn()));
	QPushButton* offButton = new QPushButton("&Off");
	QObject::connect(offButton, SIGNAL(clicked()), this, SLOT(turnOff()));

	QFrame* buttonFrame = applyLayout<QFrame, QHBoxLayout>({onButton, offButton});

	applyLayout<QWidget, QVBoxLayout>(this, {statusGroupFrame, buttonFrame});

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(getStatus()));
	timer->start(1000);

	QObject::connect(_networkManager.get(), &QNetworkAccessManager::finished, this,
	    [this](QNetworkReply* reply) { getStatusFinished(reply); });
}

KeyLightControlWindow::~KeyLightControlWindow() {}

void KeyLightControlWindow::turnOn() {
	// TODO
}

void KeyLightControlWindow::turnOff() {
	// TODO
}

void KeyLightControlWindow::getStatus() {
	if (_statusReply == nullptr) {
		std::string hostname = "192.168.87.26";
		std::string url = "http://" + hostname + ":9123/elgato/lights";

		_statusReply = _networkManager->get(QNetworkRequest(QUrl(url.c_str())));
	}
}

std::vector<KeyLightControlWindow::LightStatus> KeyLightControlWindow::parseStatus(const QString& jsonStatusText) {
	// example:
	// {
	// 	"numberOfLights":1,
	// 	"lights": [
	// 		{
	// 			"on":0,
	// 			"brightness":5,
	// 			"temperature":273
	// 		}
	// 	]
	// }

	QJsonDocument d = QJsonDocument::fromJson(jsonStatusText.toUtf8());
	QJsonObject root = d.object();

	std::vector<KeyLightControlWindow::LightStatus> lightStatus;

	QJsonArray lights = root.value(QString("lights")).toArray();
	for (QJsonValueRef lightValue : lights) {
		QJsonObject light = lightValue.toObject();
		lightStatus.push_back(LightStatus{
		    light["on"].toInt() == 0 ? false : true, light["brightness"].toInt(), light["temperature"].toInt()});
	};

	return lightStatus;
}

void KeyLightControlWindow::getStatusFinished(QNetworkReply* reply) {
	if (reply == _statusReply) {
		reply->deleteLater();

		if (reply->error()) {
			qDebug() << reply->errorString();
			return;
		}

		QString jsonStatusText = reply->readAll();

		qDebug() << "LightStatus: " << jsonStatusText;
		auto lightStatus = parseStatus(jsonStatusText);
		updateStatusUI(lightStatus);

		_statusReply = nullptr;
	}
}

void KeyLightControlWindow::updateStatusUI(std::vector<LightStatus> lightStatus) {
	//_statusFrame
}
