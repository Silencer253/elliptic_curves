#include "server.h"

void server::run()
{
    QHostAddress serverAddr( ipAddr );
    QTcpServer server;
    if (server.listen(serverAddr, port))
    {
        printf("EchoServer::run: listen() succeeded\n");
        while (server.isListening())
        {
          if (server.waitForNewConnection(100))
          {
            printf("EchoServer::run: got a TCP connection\n");
            QTcpSocket *client = server.nextPendingConnection();
            QString message;
            do
            {
               message = recieveMessage(client);
            } while (message.length() > 0);
          }
          else
            Sleep( 100 );
        }
    }
    else
        printf("EchoServer::run: listen operation failed\n");
}

int server::waitForInput(QTcpSocket *socket)
{
    int bytesAvail = -1;
    while (socket->state() == QAbstractSocket::ConnectedState && bytesAvail < 0)
    {
        if (socket->waitForReadyRead( 100 ))
            bytesAvail = socket->bytesAvailable();
        else
            Sleep( 50 );
    }
    return bytesAvail;
}

QString server::recieveMessage(QTcpSocket *client)
{
    QString message = "";
    int bytesAvail = waitForInput(client);
    if (bytesAvail > 0)
    {
        int cnt = 0;
        bool endOfStream = false;
        while (cnt < bytesAvail && (!endOfStream))
        {
          char ch;
          int bytesRead = client->read(&ch, sizeof(ch));
          if (bytesRead == sizeof(ch))
          {
            cnt++;
            message.append( ch );
          }
          else
            endOfStream = true;
        }
    }

    return message;
}

