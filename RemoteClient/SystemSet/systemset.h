#ifndef SYSTEMSET_H
#define SYSTEMSET_H

#include <QWidget>

namespace Ui {
class SystemSet;
}

class QTableWidget;
class SystemSet : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSet(QWidget *parent = nullptr);
    ~SystemSet();
signals:
    void sigSaveOk();
private:
    Ui::SystemSet *ui;
    void delAllConf();
    void initConnect();
    void initTableWidget(QTableWidget *tableWidget);
    void confHostList(QTableWidget *tableWidget);
private slots:
    void slotBtnAddHost();
    void slotBtnDelHost();
    void slotBtnSaveHost();
    void slotCellCheckClick(int row,int column);
    //void slotCellDoubleClickd(int row,int column);
};

#endif // SYSTEMSET_H
