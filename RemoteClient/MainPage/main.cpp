#include "homepage.h"
#include <QApplication>
#include <QTextCodec>
#include "guimanager.h"
int main(int argc, char *argv[])
{

    QTextCodec *codec = QTextCodec::codecForName("GBK");
//    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);
    GuiManager guiManager;
    guiManager.guiWork();

    return a.exec();
}
