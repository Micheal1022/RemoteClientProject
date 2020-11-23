#include "infohintdlg.h"
#include "ui_infohintdlg.h"

InfoHintDlg::InfoHintDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoHintDlg)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    setModal(true);


    connect(ui->tBtnClose,&QToolButton::clicked,this,&InfoHintDlg::slotBtnClose);
}

InfoHintDlg::~InfoHintDlg()
{
    delete ui;
}


void InfoHintDlg::initTalbeWidget(QTableWidget *tableWidget, InfoHintDlg::InfoType infoType)
{
    QString styleStr = "QHeaderView::section{font: 13pt '微软雅黑';background-color:rgb(53, 156, 212);"
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
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    QStringList headList;
    if (infoType == InfoType::PowerInfo || infoType == InfoType::PowerError) {
        headList<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("记录时间");
        tableWidget->setColumnCount(headList.count());
        tableWidget->setHorizontalHeaderLabels(headList);
        tableWidget->horizontalHeader()->setFixedHeight(30);
        tableWidget->verticalHeader()->setFixedWidth(40);
        tableWidget->setColumnWidth(0,120);
        tableWidget->setColumnWidth(1,120);
        tableWidget->setColumnWidth(2,120);
        tableWidget->setColumnWidth(3,140);
        tableWidget->setColumnWidth(4,200);
    } else {
        headList<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("数值")<<tr("记录时间");
        tableWidget->setColumnCount(headList.count());
        tableWidget->setHorizontalHeaderLabels(headList);
        tableWidget->horizontalHeader()->setFixedHeight(30);
        tableWidget->verticalHeader()->setFixedWidth(40);
        tableWidget->setColumnWidth(0,100);
        tableWidget->setColumnWidth(1,100);
        tableWidget->setColumnWidth(2,100);
        tableWidget->setColumnWidth(3,100);
        tableWidget->setColumnWidth(4,100);
        tableWidget->setColumnWidth(5,280);
    }
}

void InfoHintDlg::initInfoHint(InfoHintDlg::InfoType infoType, QList<QStringList> infoList)
{
    if (infoType == InfoType::AlarmInfo) {
        ui->lbInfoNum->setText(QString("报警信息 ")+QString::number(infoList.count())+QString(" 条"));
        ui->groupBox->setTitle(QString("报警信息"));
        infoListShow(ui->tableWidget,infoList);
    } else if (infoType == InfoType::ErrorInfo) {
        ui->lbInfoNum->setText(QString("故障信息 ")+QString::number(infoList.count())+QString(" 条"));
        ui->groupBox->setTitle(QString("故障信息"));
        infoListShow(ui->tableWidget,infoList);
    } else if (infoType == InfoType::PowerInfo) {
        ui->lbInfoNum->setText(QString("电源中断 ")+QString::number(infoList.count())+QString(" 条"));
        ui->groupBox->setTitle(QString("电源中断"));
        infoListShow(ui->tableWidget,infoList);
        initTalbeWidget(ui->tableWidget,infoType);
    } else if (infoType == InfoType::PowerError) {
        ui->lbInfoNum->setText(QString("电源故障 ")+QString::number(infoList.count())+QString(" 条"));
        ui->groupBox->setTitle(QString("电源故障"));
        infoListShow(ui->tableWidget,infoList);
        initTalbeWidget(ui->tableWidget,infoType);
    }
}

void InfoHintDlg::infoListShow(QTableWidget *tableWidget, QList<QStringList> infoList)
{
    tableWidget->clearContents();
    int columnCount = tableWidget->columnCount();
    //获取数据列表

    tableWidget->setRowCount(infoList.count());
    QFont ft("微软雅黑",12);
    QTableWidgetItem *item;
    for (int row = 0; row < infoList.count();row++) {
        QStringList itemStr = infoList.at(row);
        //tableWidget->setRowHeight(row,27);
        for (int column = 0;column < columnCount;column++) {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor(255,255,255));
            item->setText(itemStr.at(column));
            tableWidget->setItem(row,column,item);
        }
    }
}

void InfoHintDlg::slotBtnClose()
{
    ui->tableWidget->clearContents();
    close();
}


