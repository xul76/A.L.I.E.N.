#include "alienserver.h"

alienServer::alienServer(QObject *parent) : QObject(parent)
{
    serverSocket = new QTcpServer(this);

    connect(serverSocket,
            SIGNAL(newConnection()),
            this,
            SLOT(serverConnected())
            );
}


bool alienServer::startServer(QHostAddress address, quint16 localPort)
{
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
    // shutdown our server socket
    //    ... also prevents further connection attempts to this socket
    serverSocket->close();

    // loop through controller
    for(int i = 0; i < controller.count(); i++)
    {
        try {
            // call the disconnectFromHost function on each ALIEN client
            //    ... do i really need to fucking explain what the fuck that does ?
            controller.at(i)->clientSocket->disconnectFromHost();

            // disconnect signal handlers
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
        catch (std::exception& e){
            emit sigServerError(e.what());
        }
    }

    // reset master controller entries
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
        controller.at(a)->clientSocket->disconnectFromHost();
        controller.at(a)->zeroProtocol();
    }

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
    // loop through controller
    for (int i = 0; i < controller.count(); i++)
    {
        // find ALIEN client by socketID + socketGUID
        if (controller.at(i)->socketID == sid && controller.at(i)->socketGUID == sguid)
        {
            // convert <payload> to UTF-8 encoded byte array as <buf>
            QByteArray buf = payload.toUtf8();
            
            // send <buf> to endpoint
            controller.at(i)->clientSocket->write(buf);
        }
    }
}


void alienServer::serverConnected()
{
    // create new ALIEN client on new connection attempt
    //    ... all communications to and from this endpoint goes through this object
    alienClient *nsock = new alienClient(this);

    // new ALIEN client parent must call init() function
    //    ... init() sets up basic vars for the socket
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

    // add new ALIEN client to controller
    controller.append(nsock);

    emit sigClientConnected(nsock->socketID, nsock->socketGUID);
}

void alienServer::serverDisconnected(QString sid, QString sguid)
{
    QList<int> toRemove;

    for(int i = 0; i < controller.count(); i++)
    {
        // find ALIEN client tied to this end point by socketID and socketGUID
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

void alienServer::serverDataReceived(QString sid, QString sguid, QString payload)
{
    emit sigClientDataReceived(sid, sguid, payload);
}
