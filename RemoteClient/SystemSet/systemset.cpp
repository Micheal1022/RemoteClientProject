#include "systemset.h"
#include "ui_systemset.h"

#include <QDebug>
#include <QTableWidget>
#include <QMessageBox>

#include "SqlManager/sqlmanager.h"

#define HOST    0
#define PORT    1
#define NUM     2
#define ABLE    3
#define ADDRESS 4

SystemSet::SystemSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSet)
{
    ui->setupUi(this);
    ui->tBtnAdd->setIcon(QIcon(QPixmap(":/Image/addDev.png")));
    ui->tBtnDel->setIcon(QIcon(QPixmap(":/Image/delDev.png")));
    ui->tBtnSave->setIcon(QIcon(QPixmap(":/Image/saveConf.png")));
    ui->tBtnAdd->setIconSize(QSize(30,30));
    ui->tBtnDel->setIconSize(QSize(30,30));
    ui->tBtnSave->setIconSize(QSize(30,30));


    QRegExp regExpHostPort("^([1-9][0-9][0-9][0-9])$");//1-9999
    ui->lEditHostPort->setValidator(new QRegExpValidator(regExpHostPort, this));

    QRegExp regExpNodeNum("^([1-9][0-9]{1,2})|(101[0-9])|(102[0-4])$");//1-1024
    ui->lEditNodeNum->setValidator(new QRegExpValidator(regExpNodeNum, this));

    initTableWidget(ui->tableWidget);
    confHostList(ui->tableWidget);
    initConnect();
}

SystemSet::~SystemSet()
{
    delete ui;
}

void SystemSet::delAllConf()
{
    QString query = "delete from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    SqlManager::delAllData(db,query);
    SqlManager::closeConnection(db);
}

void SystemSet::initConnect()
{
    connect(ui->tBtnAdd,&QToolButton::clicked,this,&SystemSet::slotBtnAddHost);
    connect(ui->tBtnDel,&QToolButton::clicked,this,&SystemSet::slotBtnDelHost);
    connect(ui->tBtnSave,&QToolButton::clicked,this,&SystemSet::slotBtnSaveHost);
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&SystemSet::slotCellCheckClick);
    //connect(ui->tableWidget,&QTableWidget::cellDoubleClicked,this,&SystemSet::slotCellDoubleClickd);
}

void SystemSet::initTableWidget(QTableWidget *tableWidget)
{
    QString styleStr = "QHeaderView::section{font: 16pt '楷体';background-color:rgb(53, 156, 212);"
                       "color: black;border: 1px solid #6c6c6c;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    //tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    QStringList headList;
    headList<<tr("主机IP地址")<<tr("主机端口号")<<tr("节点数目")<<tr("启用状态")<<tr("安装地址");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(22);

    tableWidget->setColumnWidth(0,200);
    tableWidget->setColumnWidth(1,120);
    tableWidget->setColumnWidth(2,90);
    tableWidget->setColumnWidth(3,90);
    tableWidget->setColumnWidth(4,300);

}

void SystemSet::confHostList(QTableWidget *tableWidget)
{
    int columnCount = tableWidget->columnCount();
    QString query = "select HOST,PORT,NUM,ABLE,ADDRESS from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::HostList);
    SqlManager::closeConnection(db);

    tableWidget->setRowCount(nodeList.count());
    QFont ft("楷体",14);
    QTableWidgetItem *item;
    for(int row = 0; row < nodeList.count();row++)
    {
        QStringList itemStr = nodeList.at(row);
        QString host,port,number,able,address;
        host    = itemStr.at(0);
        port    = itemStr.at(1);
        number  = itemStr.at(2);
        able    = itemStr.at(3);
        address = itemStr.at(4);
        //tableWidget->setRowHeight(row,27);

        for(int column = 0;column < columnCount;column++)
        {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor("white"));

            switch (column)
            {
            case HOST://主机地址
                item->setText(host);
                break;
            case PORT://端口
                item->setText(port);
                break;
            case NUM:
                item->setText(number);
                break;
            case ABLE://功能
                if(able.toInt() == 1)
                {
                    item->setText("启用");
                    item->setCheckState(Qt::Checked);
                }
                else
                {
                    item->setText("禁用");
                    item->setCheckState(Qt::Unchecked);
                }
                break;
            case ADDRESS://地址
                item->setText(address);
                break;
            default:
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void SystemSet::slotBtnAddHost()
{
    //判断是否会出现相同的主机地址
    QString hostIP = ui->lEditHostIP->text();
    for(int row = 0; row < ui->tableWidget->rowCount();row++){
        if(ui->tableWidget->item(row,0)->text() == hostIP)
        {
            QMessageBox::critical(this,"错误提示","主机地址中有相同的IP","关闭");
            return;
        }
    }
    QString hostPort = ui->lEditHostPort->text();
    QString nodeNumber = ui->lEditNodeNum->text();
    QString hostAddress = ui->lEditHostAddress->text();

    if(hostAddress.isEmpty())
    {
        hostAddress+="-";
    }

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);

    QFont ft("楷体",14);
    QTableWidgetItem *item;
    for(int column = 0;column < ui->tableWidget->columnCount();column++)
    {
        item = new QTableWidgetItem;
        item->setFont(ft);
        item->setTextColor(QColor("white"));
        item->setTextAlignment(Qt::AlignCenter);
        switch (column) {
        case HOST:
            item->setText(hostIP);
            break;
        case PORT:
            item->setText(hostPort);
            break;
        case NUM:
            item->setText(nodeNumber);
            break;
        case ABLE:
            item->setCheckState(Qt::Checked);
            item->setText("启用");
            break;
        case ADDRESS:
            item->setText(hostAddress);
            break;
        default:
            break;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,column,item);
    }
}

void SystemSet::slotBtnDelHost()
{
    int row = ui->tableWidget->currentRow();
    if(row >= 0 && ui->tableWidget->rowCount() != 1)
    {
        QString host = ui->tableWidget->item(row,0)->text();
        QString port = ui->tableWidget->item(row,1)->text();
        ui->tableWidget->removeRow(row);
        QSqlDatabase db = SqlManager::openConnection();
        bool flag = SqlManager::delelteHostItem(db,host,port);
        SqlManager::closeConnection(db);

        if(flag)
        {
            QMessageBox::information(this,QStringLiteral("删除提示"),QStringLiteral("该条信息已经删除!"),QStringLiteral("关闭"));
        }
    }
    else
    {
        QMessageBox::information(this,QStringLiteral("删除提示"),QStringLiteral("地址信息至少要有一条!"),QStringLiteral("关闭"));
    }
}

void SystemSet::slotBtnSaveHost()
{
    int rowCount = ui->tableWidget->rowCount();
    if(rowCount > 0)
    {
        delAllConf();
        for(int row = 0;row < rowCount;row++)
        {
            QString host = ui->tableWidget->item(row,HOST)->text();
            QString port = ui->tableWidget->item(row,PORT)->text();
            QString num  = ui->tableWidget->item(row,NUM)->text();
            QString able;
            if(ui->tableWidget->item(row,ABLE)->checkState() == Qt::Checked)
            {
                able = QString("1");
            }
            else
            {
                able = QString("0");
            }
            QString address = ui->tableWidget->item(row,ADDRESS)->text();
            QSqlDatabase db = SqlManager::openConnection();
            bool flag = SqlManager::insertHostList(db,host,port,num,able,address);
            SqlManager::closeConnection(db);
            if(!flag)
            {
                QMessageBox::warning(this,"保存提示","信息保存失败！","关闭");
                return;
            }
        }
        QMessageBox::warning(this,"保存提示","信息保存成功！","关闭");
        emit sigSaveOk();
    }
}

void SystemSet::slotCellCheckClick(int row, int column)
{
    if(column == ABLE)
    {
        if(ui->tableWidget->item(row,column)->checkState() == Qt::Unchecked)
        {
            ui->tableWidget->item(row,column)->setText(tr("启用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget->item(row,column)->setText(tr("禁用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
        }
    }
}

//void SystemSet::slotCellDoubleClickd(int row, int column)
//{
//    Q_UNUSED(row)
//    Q_UNUSED(column)
////    if(column != 0 && column != 3)
////    {
////        ui->tableWidget->item(row,column)->setFlags(Qt::ItemIsEditable);
////    }
//}
