#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QWidget>

namespace Ui {
class SystemInfo;
}

class SystemInfo : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInfo(QWidget *parent = 0);
    ~SystemInfo();

private:
    Ui::SystemInfo *ui;
};

#endif // SYSTEMINFO_H
