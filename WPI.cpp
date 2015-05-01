#include "WPI.h"
#include "ui_WPI.h"

WPI::WPI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WPI)
{
    //WArchive(new QFile("xsystray-0_1_1.wpi"));
    try{
        this->arc=new WArchive(new QFile("xwp-1-0-8.exe"));
    } catch(exception *e){
        qDebug(e->what());
        return;
    }

    ui->setupUi(this);
}

WPI::~WPI(){
    delete ui;
}

void WPI::on_actionTest_triggered(){
    this->arc->test();
}
