#include "WPI.h"
#include "ui_WPI.h"

WPI::WPI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WPI)
{
    //WArchive(new QFile("xsystray-0_1_1.wpi"));
    try{
        this->arc=new WArchive(new QFile("cppcheck-1.66-dev-os2.wpi"));
        this->arc->install("{test:\"yay!\"}");
    } catch(exception &e){
        qDebug(e.what());
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
