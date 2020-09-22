#ifndef DISPLAYCONTAINER_H
#define DISPLAYCONTAINER_H

#include <QWidget>
namespace Ui {
class DisplayContainer;
}
class QListWidgetItem;
class QThread;
class DisplayContainer : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayContainer(QWidget *parent = 0);
    ~DisplayContainer();
    void confContainer();

private:
    Ui::DisplayContainer *ui;
    QList<QWidget *> m_widgetList;
    QList<QListWidgetItem *>m_itemList;
    QList<QThread *> m_threadList;


    void deleteList();
private slots:

    void slotCurrentStacked(int row);

public slots:
    void slotSaveOk();
};
/*************************************/



#endif // DISPLAYCONTAINER_H
