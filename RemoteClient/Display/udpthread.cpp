#include "udpthread.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QUdpSocket>
#include <QThread>
#include <QNetworkInterface>
#define TIMEOUT 1000
UdpThread::UdpThread(QObject *parent) : QObject(parent)
{
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpThread::slotReadReady);
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&UdpThread::slotTimeOut,Qt::DirectConnection);
    //m_timer->start(TIMEOUT);
}


UdpThread::~UdpThread()
{
    delete m_udpSocket;
    m_udpSocket = nullptr;

}

void UdpThread::initConf(const QHostAddress &host, quint16 port)
{
    m_host = host;
    m_port = port;
    //QString localIP = getHostIpAddress();
    //m_udpSocket->bind(QHostAddress(localIP),m_port);
    m_udpSocket->bind(QHostAddress::Any,m_port);
}


void UdpThread::sendDate(QByteArray byteArray)
{
    //目标地址
    m_udpSocket->writeDatagram(byteArray,m_host,m_port);
}

QString UdpThread::getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            strIpAddress = ipAddressesList.at(i).toString();
            break;
        }
    }     // 如果没有找到，则以本地IP地址为IP
    if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return strIpAddress;
}

void UdpThread::slotBtnMute()
{
    QByteArray byteArray;
    byteArray.append(1);
    sendDate(byteArray);
}

void UdpThread::slotBtnReset()
{
    QByteArray byteArray;
    byteArray.append(2);
    sendDate(byteArray);
}

void UdpThread::slotReadReady()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        sigNodeData(datagram);
    }
}

void UdpThread::slotTimeOut()
{
    QByteArray data;
    data.append('1');
    sendDate(data);
}
