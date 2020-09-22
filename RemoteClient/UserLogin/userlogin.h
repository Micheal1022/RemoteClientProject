#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QDialog>

namespace Ui {
class UserLogin;
}

class UserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();

    void confShow();
    bool m_state;
private:
    Ui::UserLogin *ui;


signals:
    void sigUserLoginOk(bool state);

private slots:
    void slotBtnOk();
    void slotBtnBack();
};

#endif // USERLOGIN_H
