#include "alienserver.h"

alienServer::alienServer(QObject *parent) : QObject(parent)
{
    // initialise our server socket with this class as it's parent.
    serverSocket = new QTcpServer(this);
    
    // connect socket::newConnection() signal to serverConnected() function.
    connect(serverSocket,
            SIGNAL(newConnection()),
            this,
            SLOT(serverConnected())
            );
}

bool alienServer::startServer(QHostAddress address, int localPort)
{
    // # opRes:
    //   true:  socket is now successfully listening on port <localPort>.
    //   false: socket could not be started on <interface/port>.
    bool opres = false;
    
    opres = serverSocket->listen(address, localPort);
    
    if(opres == true)
    {
        adapterIP = address.toString();
    }
    
    return opres;
}

void alienServer::stopServer()
{
    // close server socket (and drop all existing connections)
    serverSocket->close();
    
    // loop through C
    for(int i = 0; i < controller.count(); i++)
    {
        controller.at(i)->shutdown();
        
        disconnect(controller.at(i)->clientSocket,
                   SIGNAL(sigClientDisconnected(QString, QString)),
                   this,
                   SLOT(serverDisconnected(QString, QString))
                   );
        
        disconnect(controller.at(i)->clientSocket,
                   SIGNAL(sigClientDataReceived(QString, QString, QString)),
                   this,
                   SLOT(serverDataReceived(QString, QString, QString))
                   );
        
        controller.at(i)->zeroProtocol();
    }
    
    qDeleteAll(controller.begin(), controller.end());
    
    controller.clear();
}

QStringList alienServer::getNetworkInterfaces()
{
    QStringList faces;
    
    QList<QNetworkInterface> interfaces =  QNetworkInterface::allInterfaces();
    
    for(int a = 0; a < interfaces.count(); ++a)
    {
        foreach (QNetworkAddressEntry e, interfaces.at(a).addressEntries())
        {
            faces.append(( interfaces.at(a).name() + ": " + e.ip().toString() ));
        }
    }
    
    return faces;
}

void alienServer::zeroProtocol()
{
    for(int a = 0; a < controller.size(); ++a)
    {
        controller.at(a)->shutdown();
        controller.at(a)->zeroProtocol();
    }
    
    qDeleteAll(controller.begin(), controller.end());

    controller.clear();
    
    disconnect(serverSocket,
               SIGNAL(newConnection()),
               this,
               SLOT(serverConnected())
               );
    
    delete serverSocket;
}

void alienServer::sendData(QString sid, QString sguid, QString payload)
{
    foreach(alienClient *entry, controller)
    {
        if (entry->socketID == sid && entry->socketGUID == sguid)
        {
            // convert <payload> to UTF-8 encoded byte array
            QByteArray buf = payload.toUtf8();

            // send <buf> to endpoint
            entry->clientSocket->write(buf);

            return;
        }
    }
}

// # server socket : connection established
void alienServer::serverConnected()
{
    alienClient *nsock = new alienClient(this);
    
    nsock->init(serverSocket->nextPendingConnection());
    
    connect(nsock,
            SIGNAL(sigDisconnected(QString, QString)),
            this,
            SLOT(serverDisconnected(QString, QString))
            );
    
    connect(nsock,
            SIGNAL(sigDataReceived(QString, QString, QString)),
            this,
            SLOT(serverDataReceived(QString, QString, QString))
            );

    // add client socket to <controller> for management purposes
    controller.append(nsock);
    
    emit sigClientConnected(nsock->socketID, nsock->socketGUID);
}

// # server socket : connection lost
void alienServer::serverDisconnected(QString sid, QString sguid)
{
    QList<int> toRemove;
    
    for(int i = 0; i < controller.count(); i++)
    {
        if(controller[i]->socketID == sid && controller[i]->socketGUID == sguid)
        {
            toRemove.append(i);
        }
    }

    for(int j = 0; j < toRemove.count(); ++j)
    {
        delete controller[toRemove.at(j)];
        controller.removeAt(toRemove.at(j));
        emit sigClientDisconnected(sid, sguid);
    }
}

// # server socket : data received
void alienServer::serverDataReceived(QString sid, QString sguid, QString payload)
{
    emit sigClientDataReceived(sid, sguid, payload);
}



















