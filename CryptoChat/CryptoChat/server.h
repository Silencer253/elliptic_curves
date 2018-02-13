#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "windows.h"
#include "QMutex"
#include "QThread"
#include "QTcpServer"
#include "QTcpSocket"
#include "socketthreadbase.h"

class server : public QObject
{
    Q_OBJECT
public:
    server(const QString &ipAddr, const ushort port) : ipAddr(ipAddr), port(port) {}
    QString recieveMessage(QTcpSocket *client);
    void sendMessage(QTcpSocket *client, const QString &line);

protected:
    const QString ipAddr;
    const ushort port;
    void run();
    int waitForInput( QTcpSocket *socket );
};

#endif // ECHOSERVER_H
