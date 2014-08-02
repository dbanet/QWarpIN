#include "wpi.h"
#include "ui_wpi.h"
#include "warchive.h"

WPI::WPI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WPI)
{
    //WArchive(new QFile("xsystray-0_1_1.wpi"));
    WArchive(new QFile("xwp-1-0-8.exe"));
    ui->setupUi(this);
}

WPI::~WPI()
{
    delete ui;
}
