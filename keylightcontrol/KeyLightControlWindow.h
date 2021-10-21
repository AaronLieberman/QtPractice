#pragma once

#include <QMainWindow>
#include <QtGui>

#include <memory>

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
	void getStatusFinished(QNetworkReply* reply);

	std::unique_ptr<QNetworkAccessManager> _networkManager;
	QNetworkReply* _statusReply = nullptr;
};
