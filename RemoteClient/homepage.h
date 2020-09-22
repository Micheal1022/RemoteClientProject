#ifndef HomePage_H
#define HomePage_H

#include <QMainWindow>
namespace Ui {
class HomePage;
}


class QTimer;

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = 0);
    ~HomePage();

private:
    Ui::HomePage *ui;
    QTimer *m_systemTime;


    void initWidget();
    void initConnect();

private slots:
    void slotSystemTimer();
};

#endif // HomePage_H
