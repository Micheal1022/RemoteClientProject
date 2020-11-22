#include "homepage.h"
#include "ui_homepage.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "Display/displaycontainer.h"
#include <UserLogin/userlogin.h>

#define QUITTIME    60*60*2
//#define QUITTIME    6
HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
    showFullScreen();
    initWidget();
    initConnect();
    //QString str = "1-123\nVA3";
    //int index = str.indexOf("\n");
    //str = str.left(index);
    //qDebug()<<"index : "<<index;
    //qDebug()<<"str   : "<<str;


}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::stackedAddWidget(QWidget *widget)
{
    /*
     *  1-pageMornitor
     *  2-pageSystemSet
     *  3-pageRecord
     *  4-pageSystemInfo
     */
    ui->stackedWidget->addWidget(widget);
}

void HomePage::initWidget()
{
    m_quitTimes = 0;
    ui->tBtnClose->setEnabled(false);
    ui->tBtnRecord->setEnabled(false);
    ui->tBtnMornitor->setEnabled(false);
    ui->tBtnSystemSet->setEnabled(false);
    m_systemTime = new QTimer;
    m_userLgoin = new UserLogin;

}

void HomePage::initConnect()
{
    connect(m_systemTime,&QTimer::timeout,this,&HomePage::slotSystemTimer);
    m_systemTime->start(1000);
    connect(m_userLgoin,&UserLogin::sigUserLoginOk,this,&HomePage::slotUserLogin);

    connect(ui->tBtnRecord,&QToolButton::clicked,this,&HomePage::slotBtnRecord);
    connect(ui->tBtnUser,&QToolButton::clicked,this,&HomePage::slotBtnUserLogin);
    connect(ui->tBtnClose,&QToolButton::clicked,this,&HomePage::slotBtnCloseApp);
    connect(ui->tBtnMornitor,&QToolButton::clicked,this,&HomePage::slotBtnMornitor);
    connect(ui->tBtnSystemSet,&QToolButton::clicked,this,&HomePage::slotBtnSystemSet);
    connect(ui->tBtnSystemInfo,&QToolButton::clicked,this,&HomePage::slotBtnSystemInfo);
}

void HomePage::slotSystemTimer()
{
    QString systemTimeStr = QDateTime::currentDateTime().toString("yyyy年MM月dd日/hh:mm:ss  ");
    ui->lbSystemTime->setText(systemTimeStr);
}

void HomePage::slotBtnRecord()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->groupBox->setTitle(QString("记录查询                                                                   "));
}

void HomePage::slotBtnMornitor()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->groupBox->setTitle(QString("监控信息                                                                   "));
}

void HomePage::slotBtnSystemSet()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->groupBox->setTitle(QString("网络设置                                                                   "));
}

void HomePage::slotBtnSystemInfo()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void HomePage::slotBtnCloseApp()
{
    QApplication::closeAllWindows();
}

void HomePage::slotBtnUserLogin()
{
    m_userLgoin->confShow();
}

void HomePage::slotUserLogin(bool state)
{
    if(state == true)
    {
        ui->tBtnUser->setText(tr("用户退出"));
    }
    else
    {
        ui->tBtnUser->setText(tr("用户登录"));
    }
    ui->tBtnClose->setEnabled(state);
    ui->tBtnRecord->setEnabled(state);
    ui->tBtnMornitor->setEnabled(state);
    ui->tBtnSystemSet->setEnabled(state);
}
