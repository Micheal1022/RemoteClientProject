#ifndef EFInfo_H
#define EFInfo_H

#include <QMap>
#include <QWheelEvent>
#include <QWidget>

namespace Ui {
class EFInfo;
}
class QButtonGroup;
class QTimer;
class QGridLayout;
class BtnUnit
{
public:

    bool used;
    int pass;
    int canId;
    int nodeStatus;
    int nodeType;

    int leakSet;  //漏电报警值
    int leakCur;  //电流实时
    int leakBase; //固有漏电
    int leakAlarm;//漏电设定值
    int leakAlarmLock;//漏电报警锁定值

    int tempCur;  //温度实时
    int tempSet;  //温度设定值
    int tempAlarm;//温度报警值
    int tempAlarmLock;//温度报警锁定值

    bool dropFlag;
    bool alarmFlag;
    bool errorFlag;
    bool normalFlag;
    QString nodeAddr;

    void initData(){
        used = false;
        nodeStatus = 0;
        nodeType = 0;
        leakAlarm = 0;
        leakAlarmLock = false;
        leakBase = 0;
        leakCur = 0;
        leakSet = 0;

        tempAlarm = 0;
        tempAlarmLock = false;
        tempCur = 0;
        tempSet = 0;

        dropFlag  = false;
        alarmFlag = false;
        errorFlag = false;
        normalFlag = false;
    }
};
class EFInfo : public QWidget
{
    Q_OBJECT

public:
    explicit EFInfo(QWidget *parent = nullptr);
    ~EFInfo();

    void setNodeCount(int nodeCount);
    void confHostAddr(QString host);

signals:
    void sigMute();
    void sigReset();
    void sigSetValue(int);

private:
    Ui::EFInfo *ui;

    int m_curPass;
    int m_curCanId;
    int m_errorCount;
    int m_alarmCount;

    int m_pageCount;
    int m_curPage;

    QTimer *m_timer;
    int m_connTimes;

    int m_row ;
    int m_column;

    int m_curBtn;
    int m_nodeCount;

    int m_connState;

    QString m_styleSheet;
    QString m_redStyle;
    QString m_greenStyle;
    QString m_yellowStyle;
    QString m_greyStyle;

    QString m_hostAddr;

    QButtonGroup *m_tBtnGroup;
    QList<QStringList> m_alarmList;
    QList<QStringList> m_errorList;
    QList<QMap<int ,BtnUnit> > m_btnUnitList;




    QList<BtnUnit> tBtnUnitInfoList;

    QGridLayout *gridLayout;
    void initLayout();//初始化布局
    void confMatrixBtn();
    int getPageCount(int nodeCount);

    QStringList addStringList(int pass,int canId,int type,int sts,int value,QString alarmTime);
    void delStringList(QList<QStringList> stringList,int pass,int canId,int state);
    QString modType(int type);//探测器类型
    void dataClean();//清空数据

    void showCurNodeValue(int index);
    void showNodeValue(int pass, int canId);//探测器实时数据

    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void slotBtnClick(int index);

    void slotBtnAlarmInfo();
    void slotBtnErrorInfo();
    void slotTimeOut();

    void slotBtnHeadPage();//首页
    void slotBtnTailPage(); //尾页
    void slotBtnLastPage(); //上一页
    void slotBtnNestPage(); //下一页
public slots:
    void slotBtnMute();
    void slotBtnReset();
    void slotNodeUpdate(QByteArray byteArray);


};

#endif // EFInfo_H
