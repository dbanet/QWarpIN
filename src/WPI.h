#ifndef WPI_H
#define WPI_H

#include <QMainWindow>
#include "WArchive.h"

namespace Ui {
class WPI;
}

class WPI : public QMainWindow
{
	Q_OBJECT

public:
	explicit WPI(QWidget *parent = 0);
	WArchive *arc;
	~WPI();

private slots:
	void on_actionTest_triggered();

public slots:
	void run();
	void aboutToQuit();

signals:
	void finished();

private:
	Ui::WPI *ui;
	QTextStream out;
	void quit(int rc);
};

#endif // WPI_H
