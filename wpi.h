#ifndef WPI_H
#define WPI_H

//class WArchiveInterfaceFactory;
#include <QMainWindow>

namespace Ui {
class WPI;
}

class WPI : public QMainWindow
{
    Q_OBJECT

public:
    explicit WPI(QWidget *parent = 0);
//    WArchiveInterfaceFactory *archiveInterfacesFactory;
    ~WPI();

private:
    Ui::WPI *ui;
};

#endif // WPI_H
