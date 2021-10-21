#pragma once

#include <QMainWindow>
#include <QtGui>

#include <memory>

class QFrame;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class KeyLightControlWindow : public QWidget {
	Q_OBJECT

public:
	KeyLightControlWindow();
	~KeyLightControlWindow();

private slots:
	void turnOn();
	void turnOff();
	void getStatus();

private:
	struct LightStatus {
		bool on;
		int brightness;
		int temp;
	};

	void getStatusFinished(QNetworkReply* reply);
	std::vector<LightStatus> parseStatus(const QString& jsonStatusText);
	void updateStatusUI(std::vector<LightStatus> lightStatus);

	std::unique_ptr<QNetworkAccessManager> _networkManager;
	QNetworkReply* _statusReply = nullptr;
	QFrame* _statusFrame;
};
