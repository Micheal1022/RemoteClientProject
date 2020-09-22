#include "displaycontainer.h"
#include "ui_displaycontainer.h"
#include "matrixdisplay.h"
#include "udpthread.h"
#include "SqlManager/sqlmanager.h"

#include <QThread>
#include <algorithm>
DisplayContainer::DisplayContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayContainer)
{
    ui->setupUi(this);

    m_itemList.clear();
    m_widgetList.clear();
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    confContainer();
    connect(ui->listWidget,&QListWidget::currentRowChanged,this,&DisplayContainer::slotCurrentStacked);

}

DisplayContainer::~DisplayContainer()
{
    delete ui;
}

void DisplayContainer::confContainer()
{
    QString query = "select HOST,PORT,NUM,ABLE,ADDRESS from HOSTINFO where ABLE = 1;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::HostList);
    SqlManager::closeConnection(db);

    m_itemList.clear();
    ui->listWidget->clear();
    qDeleteAll(m_widgetList.begin(), m_widgetList.end());
    m_widgetList.clear();

    QThread *t;
    foreach (t, m_threadList) {
        t->quit();
        t->wait();
    }

    qDeleteAll(m_threadList.begin(),m_threadList.end());
    m_threadList.clear();

    for(int i = 0;i < nodeList.count();i++)
    {
        QStringList hostInfo = nodeList.value(i);
        QString hostStr   = hostInfo.value(0);
        QString portStr   = hostInfo.value(1);
        QString nodeCount = hostInfo.value(2);
        QString addr      = hostInfo.value(4);
        quint16 port      = portStr.toUInt();
        //现实界面
        MatrixDisplay *matrixDisplay = new MatrixDisplay;
        matrixDisplay->setNodeCount(nodeCount.toInt());
        matrixDisplay->confHostAddr(hostStr);
        ui->stackedWidgetNode->addWidget(matrixDisplay);
        m_widgetList.append(ui->stackedWidgetNode->widget(i));

        //添加主机节点
        QListWidgetItem *item = new QListWidgetItem;
        m_itemList.append(item);
        item->setText(addr);
//        item->setToolTip(addr);

        item->setIcon(QIcon(QPixmap(":/Image/normalDev.png")));
        item->setSizeHint(QSize(180,45));
        ui->listWidget->addItem(item);

        UdpThread *udpThread = new UdpThread;
        udpThread->initConf(QHostAddress(hostStr) ,port);

        QThread *thread = new QThread;
        m_threadList.append(thread);
        udpThread->moveToThread(thread);

        connect(matrixDisplay,&MatrixDisplay::sigMute,udpThread,&UdpThread::slotBtnMute);
        connect(matrixDisplay,&MatrixDisplay::sigReset,udpThread,&UdpThread::slotBtnReset);
        connect(udpThread,&UdpThread::sigNodeData,matrixDisplay,&MatrixDisplay::slotNodeUpdate);

        connect(thread,&QThread::finished,udpThread,&UdpThread::deleteLater,Qt::DirectConnection);
        connect(thread,&QThread::finished,thread,&UdpThread::deleteLater,Qt::DirectConnection);
        //connect();

        thread->start();
    }
    if(ui->listWidget->count() != 0)
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));

}

void DisplayContainer::deleteList()
{

}

void DisplayContainer::slotCurrentStacked(int row)
{
    ui->stackedWidgetNode->setCurrentIndex(row);
}

void DisplayContainer::slotSaveOk()
{
    confContainer();
}
