#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <QObject>

class Record;
class HomePage;
class SystemSet;
class SystemInfo;
class DisplayContainer;

class GuiManager : public QObject
{
    Q_OBJECT
public:
    explicit GuiManager(QObject *parent = nullptr);
    void guiWork();
    void initConnect();

private:
    Record *m_record;
    HomePage *m_homePage;
    SystemSet *m_systemSet;
    SystemInfo *m_systemInfo;
    DisplayContainer *m_displayContaier;
signals:

public slots:
};

#endif // GUIMANAGER_H
