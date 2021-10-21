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

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(getStatus()));
	_timer->start(1000);

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

		std::vector<LightStatus> newStatus = parseStatus(jsonStatusText);
		if (newStatus != _currentLightStatus) {
			_currentLightStatus = newStatus;
			updateStatusUI();
		}

		_statusReply = nullptr;
	}
}

void KeyLightControlWindow::updateStatusUI() {
	QList<QWidget*> children = _statusFrame->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly);
	QLayout* statusLayout = _statusFrame->layout();

	for (int i = 0; i < (int)_currentLightStatus.size(); i++) {
		LightStatus light = _currentLightStatus[i];

		if (children.count() <= i) {
			QCheckBox* checkOn = new QCheckBox();
			checkOn->setObjectName("on");
			QObject::connect(checkOn, &QCheckBox::stateChanged, [this, lightIndex = i](int checkState) {
				_currentLightStatus[lightIndex].on = checkState != 0;
				updateLightState();
			});

			QSlider* progressBrightness = new QSlider(Qt::Orientation::Horizontal);
			progressBrightness->setObjectName("brightness");
			progressBrightness->setMinimum(1);
			progressBrightness->setMaximum(100);
			progressBrightness->setFixedWidth(100);
			progressBrightness->setTracking(false);
			QObject::connect(progressBrightness, &QSlider::valueChanged, [this, lightIndex = i](int value) {
				_currentLightStatus[lightIndex].brightness = value;
				updateLightState();
			});

			QSlider* progressTemperature = new QSlider(Qt::Orientation::Horizontal);
			progressTemperature->setObjectName("temp");
			progressTemperature->setMinimum(143);
			progressTemperature->setMaximum(344);
			progressTemperature->setFixedWidth(100);
			progressTemperature->setTracking(false);
			QObject::connect(progressTemperature, &QSlider::valueChanged, [this, lightIndex = i](int value) {
				_currentLightStatus[lightIndex].temp = value;
				updateLightState();
			});

			QFrame* lightStatusFrame =
			    applyLayout<QFrame, QHBoxLayout>({checkOn, progressBrightness, progressTemperature});

			statusLayout->addWidget(lightStatusFrame);
			children = _statusFrame->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly);
		}

		assert(children.count() > i);

		QCheckBox* checkOn = children[i]->findChild<QCheckBox*>("on", Qt::FindDirectChildrenOnly);
		QSignalBlocker checkOnBlocker(checkOn);
		checkOn->setChecked(light.on);

		QSlider* progressBrightness = children[i]->findChild<QSlider*>("brightness", Qt::FindDirectChildrenOnly);
		QSignalBlocker progressBrightnessBlocker(progressBrightness);
		progressBrightness->setValue(light.brightness);

		QSlider* progressTemperature = children[i]->findChild<QSlider*>("temp", Qt::FindDirectChildrenOnly);
		QSignalBlocker progressTemperatureBlocker(progressTemperature);
		progressTemperature->setValue(light.temp);
	}
}

void KeyLightControlWindow::updateLightState() {
	if (_currentLightStatus.empty()) {
		return;
	}

	// TODO: I cheated and only am supporting setting one light right now
	const LightStatus& status = _currentLightStatus.at(0);

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

	_networkManager->put(QNetworkRequest(QUrl(url.c_str())), QByteArray(payload.c_str(), payload.size()));

	// reset the timer
	_timer->stop();
	_timer->start();
}
