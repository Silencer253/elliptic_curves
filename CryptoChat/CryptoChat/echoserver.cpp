#include "echoserver.h"



void EchoServer::startThread()
{
    setRunThread( true );
    start();
}

void EchoServer::stopThread()
{
    setRunThread( false );
}

void EchoServer::run()
{
    QHostAddress serverAddr( mIP );
    QTcpServer server;
    if (server.listen(serverAddr, mPort)) {
        printf("EchoServer::run: listen() succeeded\n");
        while (server.isListening() && getRunThread()) {
          if (server.waitForNewConnection(100)) {
            printf("EchoServer::run: got a TCP connection\n");
            QTcpSocket *client = server.nextPendingConnection();
            echoServer( client );
          }
          else
            Sleep( 100 );
        } // while
    }
    else
        printf("EchoServer::run: listen operation failed\n");
}

void EchoServer::echoServer(QTcpSocket *client)
{
    QString line;
    do
    {
        line = readLine( client );
        if (line.length() > 0)
            writeLine( client, line );
    } while (line.length() > 0);
}
