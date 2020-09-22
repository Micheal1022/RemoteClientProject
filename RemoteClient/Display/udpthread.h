#ifndef UDPTHREAD_H
#define UDPTHREAD_H

#include <QObject>
#include <QHostAddress>

class QTimer;
class QUdpSocket;
class UdpThread : public QObject
{
    Q_OBJECT
public:
    explicit UdpThread(QObject *parent = nullptr);
    ~UdpThread();
    void initConf(const QHostAddress &host, quint16 port);

private:
    QTimer *m_timer;
    QUdpSocket *m_udpSocket;
    quint16 m_port;
    QHostAddress m_host;
    void sendDate(QByteArray byteArray);
    QString getHostIpAddress();

signals:
    void sigNodeData(QByteArray byteArray);
public slots:
    void slotBtnMute();
    void slotBtnReset();
private slots:
    void slotReadReady();
    void slotTimeOut();
};

#endif // UDPTHREAD_H
