#ifndef SOCKETTHREADBASE_H
#define SOCKETTHREADBASE_H

#include "windows.h"
#include "QMutex"
#include "QThread"
#include "QTcpServer"
#include "QTcpSocket"

class SocketThreadBase
{
public:
    SocketThreadBase(const QString &ipAddr, const ushort port) : mIP(ipAddr), mPort(port) {mRunThread = false;}
enum Identity {
    BAD_IDENTITY,
    SERVER,
    CLIENT
  };
virtual void startThread() = 0;
virtual void stopThread() = 0;
virtual Identity getIdentity() = 0;

protected:
  QMutex mMutex;
  const QString mIP;
  const ushort mPort;
  void setRunThread( bool newVal );
  bool getRunThread();
  void run();
  QString readLine(QTcpSocket *socket );
  int waitForInput( QTcpSocket *socket );
  void writeLine( QTcpSocket *client, const QString &line );

private:
  bool mRunThread;
};

#endif // SOCKETTHREADBASE_H
