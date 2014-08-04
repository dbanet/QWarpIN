#ifndef WPI_H
#define WPI_H

#include <QMainWindow>
#include "warchive.h"

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

private:
    Ui::WPI *ui;
};

#endif // WPI_H
