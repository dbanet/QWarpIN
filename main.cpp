#include "wpi.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    WPI w;
    w.show();

    return a.exec();
}
