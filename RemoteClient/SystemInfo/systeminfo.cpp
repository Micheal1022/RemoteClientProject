#include "systeminfo.h"
#include "ui_systeminfo.h"

SystemInfo::SystemInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemInfo)
{
    ui->setupUi(this);
}

SystemInfo::~SystemInfo()
{
    delete ui;
}
