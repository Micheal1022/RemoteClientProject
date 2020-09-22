#ifndef RECORD_H
#define RECORD_H

#include <QWidget>

namespace Ui {
class Record;
}
class QTimer;
class QTableWidget;
class Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = 0);
    ~Record();

private:
    Ui::Record *ui;
    QTimer *m_timer;
    QStringList m_hostList;
    QStringList m_addrList;
    void initHostList();
    void initTableWidget(QTableWidget *tableWidget);
    QString confQuerySql();
    void recordListShow(QTableWidget *tableWidget, QString querySql);
private slots:
    void slotBtnQuery();
    void slotBtnDelete();
    void slotBtnBackup();
    void slotUpdateTime();
    void slotBtnExport();
};

#endif // RECORD_H
