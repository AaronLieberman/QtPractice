#pragma once

#include <QMainWindow>
#include <QtGui>

#include <memory>
#include <string>

class QFrame;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class KeyLightControlWindow : public QWidget {
	Q_OBJECT

public:
	KeyLightControlWindow(std::string hostName, std::string port);
	~KeyLightControlWindow();

private slots:
	void getStatus();

private:
	struct LightStatus {
		int index;
		bool on;
		int brightness;
		int temp;
	};

	void getStatusFinished(QNetworkReply* reply);
	std::vector<LightStatus> parseStatus(const QString& jsonStatusText);
	void updateStatusUI(std::vector<LightStatus> lightStatus);
	void setLightState(LightStatus status);

	std::string _hostName;
	std::string _port;
	std::unique_ptr<QNetworkAccessManager> _networkManager;
	QNetworkReply* _statusReply = nullptr;
	QFrame* _statusFrame;
};
