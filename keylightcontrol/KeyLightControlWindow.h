#pragma once

#include <QMainWindow>
#include <QtGui>

#include <chrono>
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
		bool on;
		int brightness;
		int temp;
		bool operator==(const LightStatus& other) const {
			return on == other.on && brightness == other.brightness && temp == other.temp;
		}
	};

	void getStatusFinished(QNetworkReply* reply);
	std::vector<LightStatus> parseStatus(const QString& jsonStatusText);
	void updateStatusUI();
	void updateLightState();

	std::string _hostName;
	std::string _port;

	QTimer* _refreshTimer;
	QTimer* _sendUpdateTimer;

	std::unique_ptr<QNetworkAccessManager> _networkManager;
	QNetworkReply* _statusReply = nullptr;
	QFrame* _statusFrame;

	std::vector<LightStatus> _currentLightStatus;
	std::chrono::time_point<std::chrono::system_clock> _lastUpdateSent;
};
