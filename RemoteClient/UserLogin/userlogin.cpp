#include "userlogin.h"
#include "ui_userlogin.h"
#include <QMessageBox>
UserLogin::UserLogin(QWidget *parent) :
    QDialog(parent),m_state(false),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    setModal(true);

    connect(ui->tBtnOk,&QToolButton::clicked,this,&UserLogin::slotBtnOk);
    connect(ui->tBtnBack,&QToolButton::clicked,this,&UserLogin::slotBtnBack);
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::confShow()
{
    show();
    if (m_state == true) {
        ui->lineEditPwd->clear();
        ui->lineEditPwd->setEnabled(false);
        ui->cbBoxUserType->setEnabled(false);
        ui->groupBox->setTitle(tr("用户退出                                                                      "));

    } else {
        ui->lineEditPwd->clear();
        ui->lineEditPwd->setEnabled(true);
        ui->cbBoxUserType->setEnabled(true);
        ui->groupBox->setTitle(tr("用户登录                                                                      "));
    }
}

void UserLogin::slotBtnOk()
{
    if (m_state == false) {
        if (ui->lineEditPwd->text() == QString(tr("123456"))) {
            m_state = true;
            emit sigUserLoginOk(m_state);
            hide();
        } else {
            QMessageBox::information(this,tr("系统提示"),tr("密码错误"),tr("关闭"));
        }
    } else {
        m_state = false;
        emit sigUserLoginOk(m_state);
        hide();
    }
}

void UserLogin::slotBtnBack()
{
    this->hide();
}
