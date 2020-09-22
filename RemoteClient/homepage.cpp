#include "homepage.h"
#include "ui_homepage.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>

HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
    initWidget();
    //DisplayContainer *dc =  new DisplayContainer(ui->pageMornitor);

}

HomePage::~HomePage()
{
    qDebug()<<"delete";
    delete ui;
}

void HomePage::initWidget()
{
    m_systemTime = new QTimer;
    initConnect();

}

void HomePage::initConnect()
{
    connect(m_systemTime,&QTimer::timeout,this,&HomePage::slotSystemTimer);
    m_systemTime->start(1000);
}

void HomePage::slotSystemTimer()
{
    QString systemTimeStr = QDateTime::currentDateTime().toString("yyyy年MM月dd日/hh:mm:ss  ");
    ui->lbSystemTime->setText(systemTimeStr);
}
