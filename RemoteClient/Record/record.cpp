#include "record.h"
#include "ui_record.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QTextEdit>
#include <QTimer>

#include "SqlManager/sqlmanager.h"


#define HOST    0
#define PASS    1
#define CANID   2
#define TYPE    3
#define STATE   4
#define VALUE   5
#define TIME    6

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);
    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&Record::slotUpdateTime);
    m_timer->start(60*1000);
    ui->dTEditStop->setDateTime(QDateTime::currentDateTime());
    initHostList();
    initTableWidget(ui->tableWidget);

    connect(ui->tBtnQuery,&QToolButton::clicked,this,&Record::slotBtnQuery);
    connect(ui->tBtnDelete,&QToolButton::clicked,this,&Record::slotBtnDelete);
    connect(ui->tBtnExport,&QToolButton::clicked,this,&Record::slotBtnExport);


}

Record::~Record()
{
    delete ui;
}

void Record::initHostList()
{
    QString query = "select HOST,PORT,ABLE,ADDRESS from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::DisplayList);
    SqlManager::closeConnection(db);

    for(int i = 0;i < nodeList.count();i++)
    {
        QStringList hostInfo = nodeList.value(i);
        ui->cbBoxHost->addItem(hostInfo.value(3));

        m_hostList.append(hostInfo.value(0));
        m_addrList.append(hostInfo.value(3));

    }
}

void Record::initTableWidget(QTableWidget *tableWidget)
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
    tableWidget->setFocusPolicy(Qt::NoFocus); //去除选中虚线框
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行

    QStringList headList;
    headList<<tr("主机地址")<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("数值")<<tr("报警时间");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(40);

    tableWidget->setColumnWidth(0,250);
    tableWidget->setColumnWidth(1,100);
    tableWidget->setColumnWidth(2,100);
    tableWidget->setColumnWidth(3,100);
    tableWidget->setColumnWidth(4,100);
    tableWidget->setColumnWidth(5,250);
}

QString Record::confQuerySql()
{
    QString querySql = "select HOST,NET,ID,TYPE,STS,VALUE,TIME from RECORD where";

    QString host = m_hostList.value(ui->cbBoxHost->currentIndex());
    querySql += " HOST = '"+host+"'";

    int state = ui->cbBoxType->currentIndex();
    switch (state) {
    case 0://全部
        break;
    case 1://报警
        querySql += " and STS = 1";
        break;
    case 2://故障
        querySql += " and STS = 2";
        break;
    case 3://掉线
        querySql += " and STS = 3";
        break;
    default:
        break;
    }
    QString startTime = QString::number(ui->dTEditStart->dateTime().toTime_t());
    QString stopTime  = QString::number(ui->dTEditStop->dateTime().toTime_t());

    querySql += " and TIME between "+startTime+" and "+stopTime+" order by TIME desc;";
    return querySql;
}

void Record::recordListShow(QTableWidget *tableWidget, QString querySql)
{
    tableWidget->clearContents();
    int columnCount = tableWidget->columnCount();
    //获取数据列表
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,querySql,SqlManager::RecordList);
    SqlManager::closeConnection(db);

    tableWidget->setRowCount(nodeList.count());
    QFont ft("楷体",14);
    QTableWidgetItem *item;
    for(int row = 0; row < nodeList.count();row++)
    {
        QStringList itemStr = nodeList.at(row);
        QString host,pass,canId,type,state,value,time;
        host  = itemStr.at(0);
        pass  = itemStr.at(1);
        canId = itemStr.at(2);
        type  = itemStr.at(3);
        state = itemStr.at(4);
        value = itemStr.at(5);
        time  = itemStr.at(6);
        //tableWidget->setRowHeight(row,27);

        for(int column = 0;column < columnCount;column++)
        {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor(255,255,255));

            switch (column)
            {
            case HOST://主机地址
                item->setText(m_addrList.at(ui->cbBoxHost->currentIndex()));
                break;
            case PASS://通道
                item->setText(pass);
                break;
            case CANID://地址
                item->setText(canId);
                break;
            case TYPE://类型
                if(type.toInt() == 2)//漏电
                {
                    item->setText("漏电");
                }
                else//温度
                {
                    item->setText("温度");
                }
                break;
            case STATE://状态
                switch (state.toInt()) {
                case 1://报警
                    item->setText("模块报警");
                    break;
                case 2://故障
                    item->setText("模块故障");
                    break;
                case 3://掉线
                    item->setText("模块掉线");
                    break;
                default:
                    break;
                }
                break;
            case VALUE://数值
                if(type.toInt() == 2 && state.toInt() == 1)//漏电
                {
                    item->setText(value+"mA");
                }
                else if(type.toInt() == 3 && state.toInt() == 1)//温度
                {
                    item->setText(value+"℃");
                }
                else
                {
                    item->setText("--");
                }
                break;
            case TIME://时间
                item->setText(QDateTime::fromTime_t(time.toUInt()).toString("yyyy年MM月dd hh:mm:ss"));
                break;
            default:
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void Record::slotBtnQuery()
{
    QString querySql = confQuerySql();
    recordListShow(ui->tableWidget,querySql);
}

void Record::slotBtnDelete()
{
    int ret = QMessageBox::question(this,"历史信息删除","你确定要删除全部的历史信息吗？","取消","确定");
    if(ret)
    {
        QString sql = "delete from RECORD;";
        QSqlDatabase db = SqlManager::openConnection();
        bool delFlag = SqlManager::delAllData(db,sql);
        SqlManager::closeConnection(db);

        if(delFlag)
        {
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);
        }
    }
}

void Record::slotBtnBackup()
{

}

void Record::slotUpdateTime()
{
    ui->dTEditStop->setDateTime(QDateTime::currentDateTime());
}

void Record::slotBtnExport()
{
    QString fileName = QString(tr("历史记录_")+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm"));
    QString filePath = QFileDialog::getSaveFileName(this,tr("历史记录导出..."),fileName,tr("EXCEL files(*.xls);;HTML-Files(*.txt);;"));
    if (filePath.isEmpty()) {
        return;
    }
    int row = ui->tableWidget->rowCount();
    int column = ui->tableWidget->columnCount();
    QList<QString> list;
    //添加列标题
    QString HeaderRow;
    for (int i=0;i<column;i++) {
        HeaderRow.append(ui->tableWidget->horizontalHeaderItem(i)->text()+"\t");
    }
    list.append(HeaderRow);
    //添加item内容
    for(int i=0;i<row;i++) {
        QString rowStr = "";
        for(int j=0;j<column;j++) {
            rowStr += ui->tableWidget->item(i,j)->text() + "\t";
        }
        list.append(rowStr);
    }
    QTextEdit textEdit;
    for(int i=0;i<list.size();i++) {
        textEdit.append(list.at(i));
    }
    //文件保存
    QFile file(filePath);
    if(file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        ts<<textEdit.document()->toPlainText();
        file.close();
    }
    QMessageBox::information(this,tr("记录导出"),tr("已经将所有数据导出"),tr("关闭"));
}
