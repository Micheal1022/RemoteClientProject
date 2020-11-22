#include "EFInfo.h"
#include "ui_EFInfo.h"
#include <QDebug>
#include <QMessageBox>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTimer>
#include <QButtonGroup>
#include <QTextCodec>
#include "infohintdlg.h"
#include "SqlManager/sqlmanager.h"


#define CONN_NORMAL 1
#define CONN_ERROR  2

#define NODENUM 120
#define ICON_SIZE 25

#define INDEX_H      0
#define INDEX_L      1
#define PASS         2
#define CANID_H      3
#define CANID_L      4
#define TYPE         5
#define STATE        6
#define CURVALUE_H   7
#define CURVALUE_L   8
#define ALARMVALUE_H 9
#define ALARMVALUE_L 10
#define BASEVALUE_H  11
#define BASEVALUE_L  12

#define NORMAL 0
#define ALARM  1
#define ERROR  2
#define DROPED 3

#define LEAK 2
#define TEMP 3

#define COLUMNSIZE 15
#define PAGESIZE 515




EFInfo::EFInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EFInfo)
{
    ui->setupUi(this);

    ui->lb_netStatus->setText(tr("正在等待接收数据."));
    ui->lb_netStatus->setStyleSheet(tr("color:rgb(255,255,255)"));
    ui->scrollArea->installEventFilter(this);


    m_alarmList.clear();
    m_errorList.clear();

    m_curBtn = 0;
    m_curPass  = 0;
    m_curCanId = 0;
    m_curPage    = 0;
    m_pageCount  = 1;
    m_errorCount = 0;
    m_alarmCount = 0;
    m_connTimes = 0;

    m_nodeCount = 0;

    m_connState = 0;
    m_styleSheet  = "font: 13pt '楷体';color: rgb(0, 0, 0);border:1px solid;"
                    "border-radius:10px;border-color: rgb(0, 0, 0);";
    m_redStyle    = m_styleSheet+"background-color: rgb(255, 0,   0);";
    m_greenStyle  = m_styleSheet+"background-color: rgb(67, 182, 25);";
    m_yellowStyle = m_styleSheet+"background-color: rgb(255, 255, 0);";
    m_greyStyle   = m_styleSheet+"background-color: rgb(117, 117, 117);";

    ui->tBtnMute->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnReset->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnReset->setIcon(QIcon(QPixmap(":/Image/reset.png")));
    ui->tBtnMute->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnMute->setIcon(QIcon(QPixmap(":/Image/speaker.png")));
    ui->tBtnLast->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnLast->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnLast->setIcon(QIcon(QPixmap(":/Image/arrow-up.png")));
    ui->tBtnNest->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnNest->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnNest->setIcon(QIcon(QPixmap(":/Image/arrow-down.png")));
    ui->tBtnHead->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnHead->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnHead->setIcon(QIcon(QPixmap(":/Image/headPage.png")));
    ui->tBtnTail->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnTail->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnTail->setIcon(QIcon(QPixmap(":/Image/tailPage.png")));
    ui->tBtnError->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnError->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnError->setIcon(QIcon(QPixmap(":/Image/errorInfo.png")));
    ui->tBtnAlarm->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tBtnAlarm->setIconSize(QSize(ICON_SIZE,ICON_SIZE));
    ui->tBtnAlarm->setIcon(QIcon(QPixmap(":/Image/alarmInfo.png")));

    connect(ui->tBtnAlarm,&QToolButton::clicked,this,&EFInfo::slotBtnAlarmInfo);
    connect(ui->tBtnError,&QToolButton::clicked,this,&EFInfo::slotBtnErrorInfo);
    connect(ui->tBtnHead, &QToolButton::clicked,this,&EFInfo::slotBtnHeadPage);
    connect(ui->tBtnTail, &QToolButton::clicked,this,&EFInfo::slotBtnTailPage);
    connect(ui->tBtnLast, &QToolButton::clicked,this,&EFInfo::slotBtnLastPage);
    connect(ui->tBtnNest, &QToolButton::clicked,this,&EFInfo::slotBtnNestPage);


    connect(ui->tBtnMute,&QToolButton::clicked,this,&EFInfo::slotBtnMute);
    connect(ui->tBtnReset,&QToolButton::clicked,this,&EFInfo::slotBtnReset);
    connect(this,SIGNAL(sigSetValue(int)),ui->scrollArea->verticalScrollBar(),SLOT(setValue(int)));

    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&EFInfo::slotTimeOut);
    m_timer->start(300);

}

bool EFInfo::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->scrollArea) {
        if (event->type() == QEvent::Wheel) {
            event->ignore();
            return true;
        }
    }
    return true;

}


EFInfo::~EFInfo()
{
    delete ui;
}

void EFInfo::setNodeCount(int nodeCount)
{
    m_nodeCount = nodeCount;
    m_pageCount = getPageCount(nodeCount) - 1;
    if (m_pageCount == 0) {
        ui->tBtnHead->setEnabled(false);
        ui->tBtnLast->setEnabled(false);
        ui->tBtnTail->setEnabled(false);
        ui->tBtnNest->setEnabled(false);
    } else  {
        ui->tBtnHead->setEnabled(false);
        ui->tBtnLast->setEnabled(false);
        ui->tBtnTail->setEnabled(true);
        ui->tBtnNest->setEnabled(true);
    }
    initLayout();
    confMatrixBtn();

}

void EFInfo::initLayout()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setSpacing(5);
    verticalLayout->setContentsMargins(5, 5, 5, 5);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(5);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(14);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    gridLayout->setContentsMargins(15, 5, 5, 5);

    horizontalLayout->addLayout(gridLayout);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout->addLayout(horizontalLayout);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    //ui->scrollArea->verticalScrollBar()->setSingleStep(55*10);

    m_tBtnGroup = new QButtonGroup;
    connect(m_tBtnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void EFInfo::confMatrixBtn()
{
    tBtnUnitInfoList.clear();
    while(gridLayout->count() != 0)
    {
        QWidget *w = gridLayout->takeAt(0)->widget();
        gridLayout->removeWidget(w);
        delete w;
    }

    for(int ind = 0;ind < m_nodeCount;ind++)
    {
        QToolButton *tBtn = new QToolButton(ui->scrollAreaWidgetContents);
        tBtn->setFocusPolicy(Qt::ClickFocus);
        tBtn->setMinimumSize(QSize(70, 50));
        tBtn->setMaximumSize(QSize(70, 50));

        BtnUnit btnUnitInfo;
        btnUnitInfo.initData();
        QString nodeStr = QString("未注册");
        //QString nodeStr = QString::number(ind+1);
        tBtnUnitInfoList.append(btnUnitInfo);
        tBtn->setText(nodeStr);
        m_column = ind % COLUMNSIZE;
        m_row = ind / COLUMNSIZE;
        gridLayout->addWidget(tBtn, m_row, m_column, 1, 1);
        m_tBtnGroup->addButton(tBtn,ind);
    }
}

int EFInfo::getPageCount(int nodeCount)
{
    int pageCount = nodeCount / NODENUM;
    if((nodeCount % NODENUM))
    {
        pageCount += 1;
    }
    return pageCount;
}

QStringList EFInfo::addStringList(int pass, int canId, int type, int sts, int value, QString alarmTime)
{

    QString passStr = QString::number(pass);
    QString canIdStr= QString::number(canId);
    QString typeStr = (3-type) ? QString("漏电") : QString("温度");
    QString stsStr;
    QString valueStr;
    switch (sts) {
    case ALARM:
        stsStr = QString("报警");
        valueStr = QString::number(value);
        break;
    case ERROR:
        stsStr = QString("故障");
        valueStr = QString("-");
        break;
    case DROPED:
        stsStr = QString("通信故障");
        valueStr = QString("-");
        break;
    default:
        break;
    }

    QStringList strList;
    strList<<passStr<<canIdStr<<typeStr<<stsStr<<valueStr<<alarmTime;

    return strList;
}

void EFInfo::delStringList(QList<QStringList> stringList, int pass, int canId, int state)
{
    QString passStr = QString::number(pass);
    QString canIdStr= QString::number(canId);
    QString stsStr;

    switch (state) {
    case ALARM:
        stsStr = QString("报警");
        break;
    case ERROR:
        stsStr = QString("故障");
        break;
    case DROPED:
        stsStr = QString("通信故障");
        break;
    }

    for(int index = 0;index < stringList.count();index++)
    {
        if(stringList.at(index).at(0) == passStr && stringList.at(index).at(1) ==canIdStr && stringList.at(index).at(3) == stsStr)
        {
            stringList.removeAt(index);
        }
    }
}

void EFInfo::confHostAddr(QString host)
{
    m_hostAddr = host;
}


QString EFInfo::modType(int type)
{
    QString typeStr;
    switch (type) {
    case LEAK:
        typeStr = tr("\n漏电");
        break;
    case TEMP:
        typeStr = tr("\n温度");
        break;
    default:
        break;
    }
    return typeStr;
}

void EFInfo::dataClean()
{
    //温度
    ui->lcdNb_address->display(" ");
    ui->lcdNb_tempAlarm->display("0");//温度报警
    ui->lcdNb_tempCur->display("0"); //温度实时
    ui->lcdNb_tempSet->display("0");//温度设定
    //漏电
    ui->lcdNb_leakBase->display("0");//固有漏电
    ui->lcdNb_leakCur->display("0");//实时漏电
    ui->lcdNb_leakAlarm->display("0");//漏电报警
    ui->lcdNb_leakSet->display("0");//漏电设定
    ui->lb_NodeAddr->clear();
}

void EFInfo::showCurNodeValue(int index)
{
    ui->lb_NodeAddr->setText(tBtnUnitInfoList.at(index).nodeAddr);
    if (tBtnUnitInfoList.at(index).nodeType == LEAK) {
        ui->lcdNb_tempCur->display("--");
        ui->lcdNb_tempSet->display("--");
        ui->lcdNb_tempAlarm->display("--");

        ui->lcdNb_leakCur->display(tBtnUnitInfoList.at(index).leakCur);
        ui->lcdNb_leakSet->display(tBtnUnitInfoList.at(index).leakSet);
        ui->lcdNb_leakBase->display(tBtnUnitInfoList.at(index).leakBase);
        ui->lcdNb_leakAlarm->display(tBtnUnitInfoList.at(index).leakAlarmLock);
    } else {
        ui->lcdNb_leakBase->display("--");
        ui->lcdNb_leakCur->display("--");
        ui->lcdNb_leakSet->display("--");
        ui->lcdNb_leakAlarm->display("--");

        ui->lcdNb_tempCur->display(tBtnUnitInfoList.at(index).tempCur);
        ui->lcdNb_tempSet->display(tBtnUnitInfoList.at(index).tempSet);
        ui->lcdNb_tempAlarm->display(tBtnUnitInfoList.at(index).tempAlarmLock);
    }
}


void EFInfo::showNodeValue(int pass, int canId)
{

    BtnUnit btnUint = m_btnUnitList.value(pass-1).value(canId);
    switch (btnUint.nodeType) {
    case LEAK:
        ui->lcdNb_leakCur->display(btnUint.leakCur);
        ui->lcdNb_leakSet->display(btnUint.leakSet);
        ui->lcdNb_leakBase->display(btnUint.leakBase);
        ui->lcdNb_leakAlarm->display(btnUint.leakAlarm);

        ui->lcdNb_tempCur->display("0");
        ui->lcdNb_tempSet->display("0");
        ui->lcdNb_tempAlarm->display("0");
        break;
    case TEMP:

        ui->lcdNb_leakCur->display("0");
        ui->lcdNb_leakSet->display("0");
        ui->lcdNb_leakBase->display("0");
        ui->lcdNb_leakAlarm->display("0");

        ui->lcdNb_tempCur->display(btnUint.tempCur);
        ui->lcdNb_tempSet->display(btnUint.tempSet);
        ui->lcdNb_tempAlarm->display(btnUint.tempAlarm);

        break;
    default:
        break;
    }


}

void EFInfo::slotBtnClick(int index)
{
    dataClean();
    //文本长度
    int length = m_tBtnGroup->button(index)->text().length();
    QString text = m_tBtnGroup->button(index)->text();
    QString showText = text.left(length-3);
    m_curPass  = showText.left(1).toInt();
    m_curCanId = showText.right(showText.length()-2).toInt();
    ui->lcdNb_address->display(showText);
    m_curBtn = index;
}

void EFInfo::slotBtnAlarmInfo()
{
    InfoHintDlg infoHintDlg(this);
    infoHintDlg.initInfoHint(InfoHintDlg::AlarmInfo,m_alarmList);
    infoHintDlg.exec();
}

void EFInfo::slotBtnErrorInfo()
{
    InfoHintDlg infoHintDlg(this);
    infoHintDlg.initInfoHint(InfoHintDlg::ErrorInfo,m_errorList);
    infoHintDlg.exec();
}

void EFInfo::slotTimeOut()
{
    if (tBtnUnitInfoList.size() > 0) {
        showCurNodeValue(m_curBtn);
    }

    m_connTimes++;
    if (m_connTimes == 300) {
        m_connTimes = 0;
        m_connState = CONN_ERROR;
        ui->lb_netStatus->setText(tr("暂无数据接收!"));
        ui->lb_netStatus->setStyleSheet(tr("color:rgb(255,255,0)"));
    } else {
        if (m_connState != CONN_ERROR && m_connState != CONN_NORMAL) {
            switch (m_connTimes % 5) {
            case 0:
                ui->lb_netStatus->setText(tr("正在等待接收数据."));
                break;
            case 1:
                ui->lb_netStatus->setText(tr("正在等待接收数据.."));
                break;
            case 2:
                ui->lb_netStatus->setText(tr("正在等待接收数据..."));
                break;
            case 3:
                ui->lb_netStatus->setText(tr("正在等待接收数据...."));
                break;
            case 4:
                ui->lb_netStatus->setText(tr("正在等待接收数据....."));
                break;
            }
            ui->lb_netStatus->setStyleSheet(tr("color:rgb(255,255,255)"));
        }
    }
}

void EFInfo::slotBtnMute()
{
    if(QMessageBox::question(this,tr("静音操作"),tr("你确定要主机静音吗?"),tr("取消"),tr("确定")))
        emit sigMute();
}

void EFInfo::slotBtnReset()
{
    if (QMessageBox::question(this,tr("复位操作"),tr("你确定要主机复位吗?"),tr("取消"),tr("确定"))) {
        dataClean();
        emit sigReset();
        m_alarmCount = 0;
        m_errorCount = 0;
        m_alarmList.clear();
        m_errorList.clear();
        ui->lb_alarmNum->setText(QString("报警")+QString::number(m_alarmCount)+QString("个"));
        ui->lb_errorNum->setText(QString("故障")+QString::number(m_errorCount)+QString("个"));

        for(int ind = 0; ind < m_tBtnGroup->buttons().size();ind++)
        {
            m_tBtnGroup->button(ind)->setStyleSheet(m_greenStyle);
        }
        for(int i = 0; i< tBtnUnitInfoList.count();i++)
            tBtnUnitInfoList[i].initData();
    }
}

void EFInfo::slotNodeUpdate(QByteArray byteArray)
{
    m_connTimes = 0;
    quint16 index = 0;
    index = ((index | (uchar)byteArray.at(INDEX_H)) << 8 ) | (uchar)byteArray.at(INDEX_L);
    if(index >= m_nodeCount)
        return;
    m_connState = CONN_NORMAL;
    ui->lb_netStatus->setText(tr("据接收正常！"));
    ui->lb_netStatus->setStyleSheet(tr("color:rgb(0,255,0)"));
    uint pass       = (uchar)byteArray.at(PASS);
    uint canId      = (uchar)byteArray.at(CANID_H)<<8 | (uchar)byteArray.at(CANID_L);
    uint nodeType   = (uchar)byteArray.at(TYPE);
    uint nodeStatus = (uchar)byteArray.at(STATE);
    uint curValue   = (uchar)byteArray.at(CURVALUE_H)   << 8 | (uchar)byteArray.at(CURVALUE_L);
    uint alarmValue = (uchar)byteArray.at(ALARMVALUE_H) << 8 | (uchar) byteArray.at(ALARMVALUE_L);
    uint baseValue  = (uchar)byteArray.at(BASEVALUE_H)  << 8 | (uchar)byteArray.at(BASEVALUE_L);
    //显示汉字
    QByteArray nodeAddrBy = byteArray.mid(BASEVALUE_L+1,byteArray.size() - 13);
    QString nodeAddrStr = QString::fromUtf8(nodeAddrBy);
    tBtnUnitInfoList[index].nodeAddr = nodeAddrStr;

    if (nodeType == LEAK) {
        m_tBtnGroup->button(index)->setText(QString::number(pass)+"-"+QString::number(canId)+QString("\n漏电"));
        tBtnUnitInfoList[index].nodeType = nodeType;
        tBtnUnitInfoList[index].leakCur  = curValue;
        tBtnUnitInfoList[index].leakBase = baseValue;
        tBtnUnitInfoList[index].leakSet  = alarmValue;
    } else {
        m_tBtnGroup->button(index)->setText(QString::number(pass)+"-"+QString::number(canId)+QString("\n温度"));
        tBtnUnitInfoList[index].nodeType = nodeType;
        tBtnUnitInfoList[index].tempCur  = curValue;
        tBtnUnitInfoList[index].tempSet  = alarmValue;
    }

    switch (nodeStatus) {
    case NORMAL:

        if (tBtnUnitInfoList[index].dropFlag == true) {
            tBtnUnitInfoList[index].dropFlag = false;
            m_errorCount--;
            delStringList(m_errorList,pass,canId,DROPED);
        }
        if (tBtnUnitInfoList[index].errorFlag == true) {
            tBtnUnitInfoList[index].errorFlag = false;
            m_errorCount--;
            delStringList(m_errorList,pass,canId,ERROR);
        }
        if (tBtnUnitInfoList[index].alarmFlag == true) {
            tBtnUnitInfoList[index].alarmFlag = false;
            m_alarmCount--;
            delStringList(m_alarmList,pass,canId,ALARM);
        }
        if(tBtnUnitInfoList[index].dropFlag == false
                && tBtnUnitInfoList[index].errorFlag == false
                && tBtnUnitInfoList[index].alarmFlag == false) {
            tBtnUnitInfoList[index].nodeStatus = NORMAL;
            if (LEAK == nodeType) {
                tBtnUnitInfoList[index].leakAlarmLock = 0;
            } else {
                tBtnUnitInfoList[index].tempAlarmLock = 0;
            }
        }
        m_tBtnGroup->button(index)->setStyleSheet(m_greenStyle);

        break;
    case ALARM:
        if (tBtnUnitInfoList[index].alarmFlag == false) {
            tBtnUnitInfoList[index].nodeStatus = ALARM;
            tBtnUnitInfoList[index].alarmFlag = true;
            tBtnUnitInfoList[index].dropFlag  = false;
            tBtnUnitInfoList[index].errorFlag = false;
            tBtnUnitInfoList[index].normalFlag= false;

            m_alarmCount++;
            //报警列表
            QString alarmTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss");
            QStringList alarmList = addStringList(pass,canId,nodeType,nodeStatus,curValue,alarmTime);
            m_alarmList.append(alarmList);
            //报警存储
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pass,canId,nodeType,nodeStatus,curValue,QDateTime::currentDateTime().toTime_t());
            SqlManager::closeConnection(db);

            if (LEAK == nodeType) {
                tBtnUnitInfoList[index].leakAlarmLock = curValue;
            } else {
                tBtnUnitInfoList[index].tempAlarmLock = curValue;
            }
        }
        m_tBtnGroup->button(index)->setStyleSheet(m_redStyle);
        break;
    case ERROR:
        if (tBtnUnitInfoList[index].errorFlag == false) {
            tBtnUnitInfoList[index].errorFlag = true;
            tBtnUnitInfoList[index].nodeStatus = ERROR;

            tBtnUnitInfoList[index].dropFlag  = false;
            tBtnUnitInfoList[index].errorFlag = false;
            tBtnUnitInfoList[index].alarmFlag = false;
            m_errorCount++;
            //故障列表
            QString errorTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss");
            QStringList errorList = addStringList(pass,canId,nodeType,nodeStatus,0,errorTime);
            m_errorList.append(errorList);
            //故障存储
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pass,canId,nodeType,nodeStatus,0,QDateTime::currentDateTime().toTime_t());
            SqlManager::closeConnection(db);
        }
        m_tBtnGroup->button(index)->setStyleSheet(m_yellowStyle);
        break;
    case DROPED:
        if (tBtnUnitInfoList[index].dropFlag == false) {
            tBtnUnitInfoList[index].dropFlag = true;
            tBtnUnitInfoList[index].nodeStatus = DROPED;

            tBtnUnitInfoList[index].alarmFlag  = false;
            tBtnUnitInfoList[index].errorFlag = false;
            tBtnUnitInfoList[index].normalFlag= false;
            m_errorCount++;
            //掉线列表
            QString errorTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss");
            QStringList errorList = addStringList(pass,canId,nodeType,nodeStatus,0,errorTime);
            m_errorList.append(errorList);
            //掉线存储
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pass,canId,nodeType,nodeStatus,0,QDateTime::currentDateTime().toTime_t());
            SqlManager::closeConnection(db);
        }
        m_tBtnGroup->button(index)->setStyleSheet(m_greyStyle);
        break;
    }
    ui->lb_alarmNum->setText(QString("报警")+QString::number(m_alarmCount)+QString("个"));
    ui->lb_errorNum->setText(QString("故障")+QString::number(m_errorCount)+QString("个"));
}

void EFInfo::slotBtnHeadPage()
{
    m_curPage = 0;
    emit sigSetValue(0);
    ui->tBtnLast->setEnabled(false);
    ui->tBtnNest->setEnabled(true);
    ui->tBtnHead->setEnabled(false);
    ui->tBtnTail->setEnabled(true);

}

void EFInfo::slotBtnTailPage()
{
    m_curPage = m_pageCount;
    emit sigSetValue(m_curPage*PAGESIZE);
    ui->tBtnNest->setEnabled(false);
    ui->tBtnLast->setEnabled(true);
    ui->tBtnTail->setEnabled(false);
    ui->tBtnHead->setEnabled(true);
}

void EFInfo::slotBtnLastPage()
{
    m_curPage--;
    if (m_curPage > 0) {
        emit sigSetValue(m_curPage*PAGESIZE);
        ui->tBtnLast->setEnabled(true);
        ui->tBtnHead->setEnabled(true);
        ui->tBtnNest->setEnabled(true);
        ui->tBtnTail->setEnabled(true);
    } else if (m_curPage == 0) {
        emit sigSetValue(m_curPage*PAGESIZE);
        ui->tBtnLast->setEnabled(false);
        ui->tBtnHead->setEnabled(false);
        ui->tBtnNest->setEnabled(true);
        ui->tBtnTail->setEnabled(true);
    }
}

void EFInfo::slotBtnNestPage()
{
    m_curPage++;
    if (m_curPage < m_pageCount) {
        emit sigSetValue(m_curPage*PAGESIZE);
        ui->tBtnLast->setEnabled(true);
        ui->tBtnHead->setEnabled(true);
        ui->tBtnNest->setEnabled(true);
        ui->tBtnTail->setEnabled(true);
    } else if (m_curPage == m_pageCount) {
        emit sigSetValue(m_curPage*PAGESIZE);
        ui->tBtnNest->setEnabled(false);
        ui->tBtnTail->setEnabled(false);
        ui->tBtnLast->setEnabled(true);
        ui->tBtnHead->setEnabled(true);
    }
}

