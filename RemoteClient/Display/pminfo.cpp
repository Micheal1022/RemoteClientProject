#include "pminfo.h"
#include "ui_pminfo.h"
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


#define COLUMNSIZE      15
#define PAGESIZE        500
#define CONN_NORMAL     1
#define CONN_ERROR      2
#define NODENUM 120

#define NODENORMAL      0x00//正常
#define OVERCURRENT     0x01//过流
#define MISPHASE        0x02//错相
#define OVERVOLTAGE     0x03//过压
#define UNDERVOLTAGE    0x04//欠压
#define OFFLINE         0x05//通讯中断
#define INTERRUPTION    0x06//供电中断


#define N_V3            0x02//双路三相电压型
#define N_V             0x03//六路单相电压型
#define N_VA3           0x04//电压电流型
#define N_DVA           0x05//直流电压电流型
#define N_VA            0x06//单项电压电流
#define N_VN3           0x07//三项双路有零
#define N_VAN3          0x08//电压电流有零
#define N_2VAN3         0x09//两路三项电压一路三项电流

#define IND_H   0
#define IND_L   1
#define PASS    2
#define CAN_H   3
#define CAN_L   4
#define TYPE    5
#define STATE   6
#define UaH_1   7
#define UaL_1   8
#define UbH_1   9
#define UbL_1   10
#define UcH_1   11
#define UcL_1   12
#define UaH_2   13
#define UaL_2   14
#define UbH_2   15
#define UbL_2   16
#define UcH_2   17
#define UcL_2   18
#define IaH_1   19
#define IaL_1   20
#define IbH_1   21
#define IbL_1   22
#define IcH_1   23
#define IcL_1   24



PMInfo::PMInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PMInfo)
{
    ui->setupUi(this);

}

void PMInfo::setInfo(int nodeCount, QString hostAddr)
{
    m_hostAddr  = hostAddr;
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
    initWidget();

}

PMInfo::~PMInfo()
{
    delete ui;
}

void PMInfo::initWidget()
{
    dataClean(); //清空数据
    initLayout();//初始化布局
    confPmBtn(); //配置按钮
    m_styleSheet  = "font:13pt '微软雅黑';color: rgb(0, 0, 0);border:1px solid rgb(0, 0, 0);border-radius:10px;";
    m_redStyle    = m_styleSheet+"background-color: rgb(255, 0,   0);";
    m_greenStyle  = m_styleSheet+"background-color: rgb(67,  182, 25);";
    m_yellowStyle = m_styleSheet+"background-color: rgb(255, 255, 0);";
    m_greyStyle   = m_styleSheet+"background-color: rgb(117, 117, 117);";
}

void PMInfo::dataClean()
{
    ui->lcdNb_Ua_1->display("0");
    ui->lcdNb_Ub_1->display("0");
    ui->lcdNb_Uc_1->display("0");
    ui->lcdNb_Ua_2->display("0");
    ui->lcdNb_Ub_2->display("0");
    ui->lcdNb_Uc_2->display("0");
    ui->lcdNb_Ia_1->display("0");
    ui->lcdNb_Ib_1->display("0");
    ui->lcdNb_Ic_1->display("0");
}

void PMInfo::initLayout()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setSpacing(5);
    verticalLayout->setContentsMargins(5, 5, 5, 5);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(5);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    m_gridLayout = new QGridLayout();
    m_gridLayout->setHorizontalSpacing(15);
    m_gridLayout->setVerticalSpacing(14);
    m_gridLayout->setObjectName(QStringLiteral("m_gridLayout"));
    m_gridLayout->setContentsMargins(15, 5, 5, 5);

    horizontalLayout->addLayout(m_gridLayout);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout->addLayout(horizontalLayout);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    m_tBtnGroup = new QButtonGroup;
    connect(m_tBtnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void PMInfo::confPmBtn()
{
    m_PmBtnUnitList.clear();
    while(m_gridLayout->count() != 0) {
        QWidget *w = m_gridLayout->takeAt(0)->widget();
        m_gridLayout->removeWidget(w);
        delete w;
    }

    for (int ind = 0;ind < m_nodeCount;ind++) {
        QToolButton *tBtn = new QToolButton(ui->scrollAreaWidgetContents);
        tBtn->setFocusPolicy(Qt::ClickFocus);
        tBtn->setMinimumSize(QSize(70, 50));
        tBtn->setMaximumSize(QSize(70, 50));
        tBtn->setText(tr("未注册"));

        PmBtnUnit pPmBtnUnit;
        pPmBtnUnit.initData();
        m_PmBtnUnitList.append(pPmBtnUnit);
        m_column = ind % COLUMNSIZE;
        m_row = ind / COLUMNSIZE;
        m_gridLayout->addWidget(tBtn, m_row, m_column, 1, 1);
        m_tBtnGroup->addButton(tBtn,ind);
    }
}

int PMInfo::getPageCount(int nodeCount)
{
    int pPageCount = nodeCount / NODENUM;
    if ((nodeCount % NODENUM)) {
        pPageCount += 1;
    }
    return pPageCount;
}

void PMInfo::showCurNodeValue(int index)
{
    ui->lbArea->setText(m_PmBtnUnitList.at(index).m_nodeAddr);

    switch (m_PmBtnUnitList.at(index).m_nodeType) {
    case N_V:
    case N_V3:
    case N_VN3://三项双路有零
        ui->lcdNb_Ua_1->display(m_PmBtnUnitList.at(index).m_Ua_1);
        ui->lcdNb_Ub_1->display(m_PmBtnUnitList.at(index).m_Ub_1);
        ui->lcdNb_Uc_1->display(m_PmBtnUnitList.at(index).m_Uc_1);

        ui->lcdNb_Ua_2->display(m_PmBtnUnitList.at(index).m_Ua_2);
        ui->lcdNb_Ub_2->display(m_PmBtnUnitList.at(index).m_Ub_2);
        ui->lcdNb_Uc_2->display(m_PmBtnUnitList.at(index).m_Uc_2);

        ui->lcdNb_Ia_1->display(0);
        ui->lcdNb_Ib_1->display(0);
        ui->lcdNb_Ic_1->display(0);
        break;
    case N_VA:
    case N_VA3://电压电流有零
    case N_VAN3://电压电流有零
        ui->lcdNb_Ua_1->display(m_PmBtnUnitList.at(index).m_Ua_1);
        ui->lcdNb_Ub_1->display(m_PmBtnUnitList.at(index).m_Ub_1);
        ui->lcdNb_Uc_1->display(m_PmBtnUnitList.at(index).m_Uc_1);

        ui->lcdNb_Ua_2->display(0);
        ui->lcdNb_Ub_2->display(0);
        ui->lcdNb_Uc_2->display(0);

        ui->lcdNb_Ia_1->display(m_PmBtnUnitList.at(index).m_Ia_1);
        ui->lcdNb_Ib_1->display(m_PmBtnUnitList.at(index).m_Ib_1);
        ui->lcdNb_Ic_1->display(m_PmBtnUnitList.at(index).m_Ic_1);
        break;
    case N_2VAN3://两路三项电压一路三项电流
        ui->lcdNb_Ua_1->display(m_PmBtnUnitList.at(index).m_Ua_1);
        ui->lcdNb_Ub_1->display(m_PmBtnUnitList.at(index).m_Ub_1);
        ui->lcdNb_Uc_1->display(m_PmBtnUnitList.at(index).m_Uc_1);

        ui->lcdNb_Ua_2->display(m_PmBtnUnitList.at(index).m_Ua_2);
        ui->lcdNb_Ub_2->display(m_PmBtnUnitList.at(index).m_Ub_2);
        ui->lcdNb_Uc_2->display(m_PmBtnUnitList.at(index).m_Uc_2);

        ui->lcdNb_Ia_1->display(m_PmBtnUnitList.at(index).m_Ia_1);
        ui->lcdNb_Ib_1->display(m_PmBtnUnitList.at(index).m_Ib_1);
        ui->lcdNb_Ic_1->display(m_PmBtnUnitList.at(index).m_Ic_1);
        break;
    }
}

void PMInfo::slotBtnClick(int index)
{
    dataClean();
    QString pBtnText = m_tBtnGroup->button(index)->text();
    int pLength   = pBtnText.indexOf("\n");
    pBtnText = pBtnText.left(pLength);
    ui->lcdNb_NodeId->display(pBtnText);
    m_currentIndex = index;
}

void PMInfo::slotBtnPowerInfo()
{

}

void PMInfo::slotBtnErrorInfo()
{

}

void PMInfo::slotTimeOut()
{
    if (m_PmBtnUnitList.size() > 0) {
        showCurNodeValue(m_currentIndex);
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

void PMInfo::slotBtnReset()
{
    if (QMessageBox::question(this,tr("复位操作"),tr("你确定要主机复位吗?"),tr("取消"),tr("确定"))) {
        dataClean();
        m_powerCount = 0;
        m_errorCount = 0;
        m_alarmList.clear();
        m_errorList.clear();
        ui->lb_alarmNum->setText(QString("报警")+QString::number(m_powerCount)+QString("个"));
        ui->lb_errorNum->setText(QString("故障")+QString::number(m_errorCount)+QString("个"));

        for (int ind = 0; ind < m_tBtnGroup->buttons().size();ind++) {
            m_tBtnGroup->button(ind)->setStyleSheet(m_greenStyle);
        }
        for(int i = 0; i< m_PmBtnUnitList.count();i++) {
            m_PmBtnUnitList[i].initData();
        }
    }
}

void PMInfo::slotBtnHeadPage()
{
    m_curPage = 0;
    emit sigSetValue(0);
    ui->tBtnLast->setEnabled(false);
    ui->tBtnNest->setEnabled(true);
    ui->tBtnHead->setEnabled(false);
    ui->tBtnTail->setEnabled(true);
}

void PMInfo::slotBtnTailPage()
{
    m_curPage = m_pageCount;
    emit sigSetValue(m_curPage*PAGESIZE);
    ui->tBtnNest->setEnabled(false);
    ui->tBtnLast->setEnabled(true);
    ui->tBtnTail->setEnabled(false);
    ui->tBtnHead->setEnabled(true);
}

void PMInfo::slotBtnLastPage()
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

void PMInfo::slotBtnNestPage()
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

void PMInfo::slotNodeUpdate(QByteArray byteArray)
{
    m_connTimes = 0;
    quint16 pIndex = (uchar)byteArray.at(IND_H);
    pIndex = (pIndex << 8 ) | (uchar)byteArray.at(IND_L);
    if(pIndex >= m_nodeCount)
        return;
    m_connState = CONN_NORMAL;
    ui->lb_netStatus->setText(tr("据接收正常！"));
    ui->lb_netStatus->setStyleSheet(tr("color:rgb(0,255,0)"));
    quint16 pPass  = (uchar)byteArray.at(PASS);
    quint16 pCanID = (uchar)byteArray.at(CAN_H)<<8 | (uchar)byteArray.at(CAN_L);
    quint16 pState = (uchar)byteArray.at(STATE);
    quint16 pType  = (uchar)byteArray.at(TYPE);
    m_PmBtnUnitList[pIndex].m_Ua_1 = (uchar)byteArray.at(UaH_1) << 8 | (uchar)byteArray.at(UaL_1);
    m_PmBtnUnitList[pIndex].m_Ub_1 = (uchar)byteArray.at(UbH_1) << 8 | (uchar)byteArray.at(UbL_1);
    m_PmBtnUnitList[pIndex].m_Uc_1 = (uchar)byteArray.at(UcH_1) << 8 | (uchar)byteArray.at(UcL_1);
    m_PmBtnUnitList[pIndex].m_Ua_2 = (uchar)byteArray.at(UaH_2) << 8 | (uchar)byteArray.at(UaL_2);
    m_PmBtnUnitList[pIndex].m_Ub_2 = (uchar)byteArray.at(UbH_2) << 8 | (uchar)byteArray.at(UbL_2);
    m_PmBtnUnitList[pIndex].m_Uc_2 = (uchar)byteArray.at(UcH_2) << 8 | (uchar)byteArray.at(UcL_2);
    m_PmBtnUnitList[pIndex].m_Ia_1 = (qreal)byteArray.at(IaH_1) + (qreal)byteArray.at(IaL_1) / 10;
    m_PmBtnUnitList[pIndex].m_Ib_1 = (qreal)byteArray.at(IbH_1) + (qreal)byteArray.at(IbL_1) / 10;
    m_PmBtnUnitList[pIndex].m_Ic_1 = (qreal)byteArray.at(IcH_1) + (qreal)byteArray.at(IcL_1) / 10;
    //显示汉字
    int addrIndex = IcL_1 + 1;
    QByteArray pNodeAddr = byteArray.mid(addrIndex,byteArray.size() - addrIndex);
    QString nodeAddrStr = QString::fromUtf8(pNodeAddr);
    m_PmBtnUnitList[pIndex].m_nodeAddr = nodeAddrStr;
    m_PmBtnUnitList[pIndex].m_nodeType = pType;

    switch (pType) {
    case N_V3:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nV3"));
        break;
    case N_V:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nV"));
        break;
    case N_VA3:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nVA3"));
        break;
    case N_VA:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nVA"));
        break;
    case N_VN3:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nVN3"));
        break;
    case N_VAN3:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\nVAN3"));
        break;
    case N_2VAN3:
        m_tBtnGroup->button(pIndex)->setText(QString::number(pPass)+"-"+QString::number(pCanID)+QString("\n2VAN3"));
        break;
    }
    uint pCurrentTime = QDateTime::currentDateTime().toTime_t();
    switch (pState) {
    case NODENORMAL:
        if (m_PmBtnUnitList[pIndex].m_offLineFlag == true) {
            m_errorCount--;
            m_PmBtnUnitList[pIndex].m_offLineFlag = false;
        }
        if (m_PmBtnUnitList[pIndex].m_misphaseFlag == true) {
            m_errorCount--;
            m_PmBtnUnitList[pIndex].m_misphaseFlag = false;
        }
        if (m_PmBtnUnitList[pIndex].m_overCurrentFlag == true) {
            m_errorCount--;
            m_PmBtnUnitList[pIndex].m_overCurrentFlag = false;
        }
        if (m_PmBtnUnitList[pIndex].m_overVoltageFlag == true) {
            m_errorCount--;
            m_PmBtnUnitList[pIndex].m_overVoltageFlag = false;
        }
        if (m_PmBtnUnitList[pIndex].m_underVoltageFlag == true) {
            m_errorCount--;
            m_PmBtnUnitList[pIndex].m_underVoltageFlag = false;
        }
        if (m_PmBtnUnitList[pIndex].m_powerLostFlag == false) {
            m_powerCount--;
            m_PmBtnUnitList[pIndex].m_powerLostFlag = true;
        }
        if (m_PmBtnUnitList[pIndex].m_offLineFlag      == false &&
            m_PmBtnUnitList[pIndex].m_misphaseFlag     == false &&
            m_PmBtnUnitList[pIndex].m_overCurrentFlag  == false &&
            m_PmBtnUnitList[pIndex].m_overVoltageFlag  == false &&
            m_PmBtnUnitList[pIndex].m_underVoltageFlag == false &&
            m_PmBtnUnitList[pIndex].m_powerLostFlag    == false)
        {
            m_PmBtnUnitList[pIndex].m_normalFlag = true;
        }
        break;

    case OFFLINE:
        if (m_PmBtnUnitList[pIndex].m_offLineFlag == false) {
            m_errorCount++;
            m_PmBtnUnitList[pIndex].m_offLineFlag = true;
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    case MISPHASE:
        if (m_PmBtnUnitList[pIndex].m_misphaseFlag == false) {
            m_errorCount++;
            m_PmBtnUnitList[pIndex].m_misphaseFlag = true;

            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    case OVERCURRENT:
        if (m_PmBtnUnitList[pIndex].m_overCurrentFlag == false) {
            m_errorCount++;
            m_PmBtnUnitList[pIndex].m_overCurrentFlag = true;
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    case OVERVOLTAGE:
        if (m_PmBtnUnitList[pIndex].m_overVoltageFlag == false) {
            m_errorCount++;
            m_PmBtnUnitList[pIndex].m_overVoltageFlag = true;
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    case UNDERVOLTAGE:
        if (m_PmBtnUnitList[pIndex].m_underVoltageFlag == false) {
            m_errorCount++;
            m_PmBtnUnitList[pIndex].m_underVoltageFlag = true;
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    case INTERRUPTION:
        if (m_PmBtnUnitList[pIndex].m_powerLostFlag == false) {
            m_powerCount++;
            m_PmBtnUnitList[pIndex].m_powerLostFlag = true;
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,m_hostAddr,pPass,pCanID,pType,pState,0,pCurrentTime);
            SqlManager::closeConnection(db);
        }
        break;
    }
    ui->lcdNb_PowerLost->display(m_powerCount);
    ui->lcdNb_NodeError->display(m_errorCount);
}
