#ifndef PMINFO_H
#define PMINFO_H

#include <QWidget>


class PmBtnUnit{
public:

    int m_pass;
    int m_canId;
    int m_nodeStatus;
    int m_nodeType;

    bool m_offLineFlag;     //掉线
    bool m_normalFlag;      //正常
    bool m_misphaseFlag;    //缺相
    bool m_overCurrentFlag; //过流
    bool m_overVoltageFlag; //过压
    bool m_underVoltageFlag;//欠压
    bool m_powerLostFlag;   //供电中断
    QString m_nodeAddr;     //安装区域

    quint16 m_Ua_1; quint16 m_Ub_1; quint16 m_Uc_1;
    quint16 m_Ua_2; quint16 m_Ub_2; quint16 m_Uc_2;
    qreal   m_Ia_1; qreal   m_Ib_1; qreal   m_Ic_1;

    void initData(){
        m_nodeAddr.clear();
        m_nodeType     = 0;
        m_nodeStatus   = 0;
        m_offLineFlag  = false;
        m_normalFlag   = false;
        m_misphaseFlag = false;
        m_overCurrentFlag = false;
        m_overVoltageFlag = false;
        m_underVoltageFlag = false;
        m_powerLostFlag    = false;

        m_Ua_1 = 0; m_Ub_1 = 0; m_Uc_1 = 0;
        m_Ua_2 = 0; m_Ub_2 = 0; m_Uc_2 = 0;
        m_Ia_1 = 0; m_Ib_1 = 0; m_Ic_1 = 0;
    }
};



namespace Ui {
class PMInfo;
}
class QButtonGroup;
class QTimer;
class QGridLayout;
class PMInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PMInfo(QWidget *parent = 0);
    void setInfo(int nodeCount, QString hostAddr);
    ~PMInfo();

private:
    Ui::PMInfo *ui;
    int m_row ;
    int m_column;
    int m_curBtn;
    int m_currentIndex;
    int m_nodeCount;
    int m_connState;

    int m_curPass;
    int m_curCanId;
    int m_errorCount;
    int m_powerCount;

    int m_pageCount;
    int m_curPage;
    QTimer *m_timer;
    int m_connTimes;
    QString m_styleSheet;
    QString m_redStyle;
    QString m_greenStyle;
    QString m_yellowStyle;
    QString m_greyStyle;

    QString m_hostAddr;


    QGridLayout *m_gridLayout;
    QButtonGroup *m_tBtnGroup;
    QList<PmBtnUnit> m_PmBtnUnitList;

    QList<QStringList> m_powerList;
    QList<QStringList> m_errorList;
    QList<QMap<int ,PmBtnUnit>> m_btnUnitList;
private:

    void initWidget();
    void dataClean();//清空数据
    void initLayout();//初始化布局
    void confPmBtn();
    int getPageCount(int nodeCount);
    void showCurNodeValue(int index);
    QStringList addStringList(int pass, int canId, int type, int state, QString alarmTime);
    void delStringList(QList<QStringList> stringList,int pass,int canId,int state);

signals:

    void sigSetValue(int);
private slots:
    void slotBtnClick(int index);

    void slotBtnPowerInfo();
    void slotBtnErrorInfo();
    void slotTimeOut();

    void slotBtnReset();

    void slotBtnHeadPage();//首页
    void slotBtnTailPage();//尾页
    void slotBtnLastPage();//上页
    void slotBtnNestPage();//下页
public slots:
    void slotNodeUpdate(QByteArray byteArray);
};

#endif // PMINFO_H
