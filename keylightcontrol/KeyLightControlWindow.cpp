#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSlider>
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

std::string replace(const std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos) {
		return str;
	}

	// makes a copy which gets modified in place
	std::string result = str;
	result.replace(start_pos, from.length(), to);
	return result;
}
} // namespace

KeyLightControlWindow::KeyLightControlWindow(std::string hostName, std::string port)
    : _hostName(std::move(hostName))
    , _port(std::move(port))
    , _networkManager(std::make_unique<QNetworkAccessManager>()) {
	QLabel* statusLabel = new QLabel("LightStatus");
	_statusFrame = applyLayout<QFrame, QVBoxLayout>({});
	QFrame* statusGroupFrame = applyLayout<QFrame, QVBoxLayout>({statusLabel, _statusFrame});

	applyLayout<QWidget, QVBoxLayout>(this, {statusGroupFrame});

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(getStatus()));
	timer->start(1000);

	QObject::connect(_networkManager.get(), &QNetworkAccessManager::finished, this,
	    [this](QNetworkReply* reply) { getStatusFinished(reply); });
}

KeyLightControlWindow::~KeyLightControlWindow() {}

void KeyLightControlWindow::getStatus() {
	if (_statusReply == nullptr) {
		std::string url = "http://" + _hostName + ":" + _port + "/elgato/lights";
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
	int lightIndex = 0;
	for (QJsonValueRef lightValue : lights) {
		QJsonObject light = lightValue.toObject();
		lightStatus.push_back(LightStatus{lightIndex++, light["on"].toInt() == 0 ? false : true,
		    light["brightness"].toInt(), light["temperature"].toInt()});
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
	qDeleteAll(_statusFrame->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
	QLayout* statusLayout = _statusFrame->layout();

	for (LightStatus light : lightStatus) {
		QCheckBox* checkOn = new QCheckBox();
		checkOn->setChecked(light.on);
		QObject::connect(checkOn, &QCheckBox::stateChanged, [this, light](int checkState) {
			LightStatus newStatus = light;
			newStatus.on = checkState != 0;
			setLightState(newStatus);
		});

		QSlider* progressBrightness = new QSlider(Qt::Orientation::Horizontal);
		progressBrightness->setMinimum(1);
		progressBrightness->setMaximum(100);
		progressBrightness->setValue(light.brightness);
		progressBrightness->setFixedWidth(100);
		progressBrightness->setTracking(false);
		QObject::connect(progressBrightness, &QSlider::valueChanged, [this, light](int value) {
			LightStatus newStatus = light;
			newStatus.brightness = value;
			setLightState(newStatus);
		});

		QSlider* progressTemperature = new QSlider(Qt::Orientation::Horizontal);
		progressTemperature->setMinimum(143);
		progressTemperature->setMaximum(344);
		progressTemperature->setValue(light.temp);
		progressTemperature->setFixedWidth(100);
		progressTemperature->setTracking(false);
		QObject::connect(progressTemperature, &QSlider::valueChanged, [this, light](int value) {
			LightStatus newStatus = light;
			newStatus.temp = value;
			setLightState(newStatus);
		});

		QFrame* lightStatusFrame = applyLayout<QFrame, QHBoxLayout>({checkOn, progressBrightness, progressTemperature});

		statusLayout->addWidget(lightStatusFrame);
	}
}

void KeyLightControlWindow::setLightState(LightStatus status) {
	std::string payload = R"RAW({
      "lights": [
        {
          "temperature": $colorTemp,
          "brightness": $brightness,
          "on": $on
        }
      ],
      "numberOfLights": 1
    })RAW";

	payload = replace(payload, "$on", status.on ? "1" : "0");
	payload = replace(payload, "$brightness", std::to_string(status.brightness));
	payload = replace(payload, "$colorTemp", std::to_string(status.temp));

	std::string url = "http://" + _hostName + ":" + _port + "/elgato/lights";

	qDebug() << payload.c_str();

	_statusReply =
	    _networkManager->put(QNetworkRequest(QUrl(url.c_str())), QByteArray(payload.c_str(), payload.size()));
}
