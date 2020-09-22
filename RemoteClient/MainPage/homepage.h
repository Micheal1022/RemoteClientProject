#ifndef HomePage_H
#define HomePage_H

#include <QMainWindow>
namespace Ui {
class HomePage;
}

class QTimer;
class UserLogin;

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = 0);
    ~HomePage();
    void stackedAddWidget(QWidget *widget);

private:
    Ui::HomePage *ui;
    UserLogin *m_userLgoin;
    QTimer *m_systemTime;
    int m_quitTimes;

    void initWidget();
    void initConnect();

private slots:
    void slotSystemTimer();

    void slotBtnRecord();
    void slotBtnMornitor();
    void slotBtnSystemSet();
    void slotBtnSystemInfo();
    void slotBtnCloseApp();

    void slotBtnUserLogin();
    void slotUserLogin(bool state);
};

#endif // HomePage_H
