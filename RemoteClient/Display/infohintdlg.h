#ifndef INFOHINTDLG_H
#define INFOHINTDLG_H

#include <QDialog>

namespace Ui {
class InfoHintDlg;
}
class QTableWidget;
class InfoHintDlg : public QDialog
{
    Q_OBJECT

public:
    explicit InfoHintDlg(QWidget *parent = 0);
    ~InfoHintDlg();
    enum InfoType{
        AlarmInfo = 0x01,
        ErrorInfo = 0x02,
        PowerInfo = 0x03,
        PowerError= 0x04
    };
    void initTalbeWidget(QTableWidget *tableWidget, InfoType infoType);

    void initInfoHint(InfoType infoType, QList<QStringList> infoList);

private:
    Ui::InfoHintDlg *ui;
    int m_infoType;
    void infoListShow(QTableWidget *tableWidget, QList<QStringList> infoList);

private slots:
    void slotBtnClose();

};

#endif // INFOHINTDLG_H
