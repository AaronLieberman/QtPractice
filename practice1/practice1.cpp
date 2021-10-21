#include <QApplication>
#include <QTextEdit>
#include <QtGui>

int main(int argv, char** args) {
	QApplication app(argv, args);
	QTextEdit textEdit;

	textEdit.show();

	return app.exec();
}
