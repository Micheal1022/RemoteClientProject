#include "guimanager.h"
#include "Record/record.h"
#include "MainPage/homepage.h"
#include "SystemSet/systemset.h"
#include "SystemInfo/systeminfo.h"
#include "Display/displaycontainer.h"

GuiManager::GuiManager(QObject *parent) : QObject(parent)
{
    m_record = new Record;
    m_homePage = new HomePage;
    m_systemSet = new SystemSet;
//    m_systemInfo = new SystemInfo;
    m_displayContaier =  new DisplayContainer;

    m_homePage->stackedAddWidget(m_displayContaier);
    m_homePage->stackedAddWidget(m_systemSet);
    m_homePage->stackedAddWidget(m_record);
//    m_homePage->stackedAddWidget(m_systemInfo);

    //配置网络完成
    connect(m_systemSet,&SystemSet::sigSaveOk,m_displayContaier,&DisplayContainer::slotSaveOk);
}

void GuiManager::guiWork()
{
    m_homePage->show();
}

void GuiManager::initConnect()
{

}
