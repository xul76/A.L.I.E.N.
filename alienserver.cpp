/*
 *                       A.L.I.E.N. Server
 *
 *           a little interface (for) easy networking
 *
 *              https://github.com/xul76/A.L.I.E.N.
 */
#include "alienserver.h"

alienServer::alienServer(QObject *parent) : QObject(parent)
{
    // create new [server] socket handler.
    serverSocket = new QTcpServer(this);

    // connect the sS::newConnection() signal to our serverConnected() function.
    connect(serverSocket,
            SIGNAL(newConnection()),
            this,
            SLOT(serverConnected())
            );
}

bool alienServer::startServer(QHostAddress address, int localPort)
{
    // e.g.:
    // alien_server_object.startServer(QHostAddress("192.168.1.10"), 51000)
    
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
    // close sS, which disconnects all connected devices and prevents further connections to sS/your app.
    serverSocket->close();

    // loop through C
    for(int i = 0; i < controller.count(); i++)
    {
        try{
            // call the shutdown() function on each a.l.i.e.n. client
            controller.at(i)->shutdown();

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

    // reset C
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
    // loop through C
    for (int i = 0; i < controller.count(); i++)
    {
        // find a.l.i.e.n. client tied to this endpoint by <sid> and <sguid>
        if (controller.at(i)->socketID == sid && controller.at(i)->socketGUID == sguid)
        {
            // convert <payload> to UTF-8 encoded byte array
            QByteArray buf = payload.toUtf8();
            
            // write <buf> to endpoint's a.l.i.e.n. client.
            controller.at(i)->clientSocket->write(buf);
        }
    }
}

void alienServer::serverConnected()
{
    // create a new a.l.i.e.n. client.
    // all communications to/from an endpoint happen through this object.
    alienClient *nsock = new alienClient(this);

    // each new a.l.i.e.n. client parent must call the appropriate init() function. in this case, we
    // initialise this object with an existing QTcpSocket; hence this overload.
    nsock->init(serverSocket->nextPendingConnection());

    // connect the nsock::sigClientDisconnected() signal to our serverDisconnected() function. a nsock
    // emits this signal when the connection to this is lost.
    connect(nsock,
            SIGNAL(sigDisconnected(QString, QString)),
            this,
            SLOT(serverDisconnected(QString, QString))
            );

    // connect the nsock::sigClientDataReceived() signal to our serverDataReceived() function. a nsock
    // emits this signal when this endpoint sends data to your app.
    connect(nsock,
            SIGNAL(sigDataReceived(QString, QString, QString)),
            this,
            SLOT(serverDataReceived(QString, QString, QString))
            );

    // add nsock to C, so that it can be accessed/managed as needed
    controller.append(nsock);

    emit sigClientConnected(nsock->socketID, nsock->socketGUID);
}

void alienServer::serverDisconnected(QString sid, QString sguid)
{
    QList<int> toRemove;

    // loop through C
    for(int i = 0; i < controller.count(); i++)
    {
        // find a.l.i.e.n. client tied to this endpoint by <sid> and <sguid>
        if(controller[i]->socketID == sid && controller[i]->socketGUID == sguid)
        {
            toRemove.append(i);
        }
    }
    // never operate on a list while iterating it. whoever told you that was a good idea was a fucking
    // moron son.
    for(int j = 0; j < toRemove.count(); ++j)
    {
        delete controller[toRemove.at(j)];
        controller.removeAt(toRemove.at(j));
        emit sigClientDisconnected(sid, sguid);
    }
    // :3
}

void alienServer::serverDataReceived(QString sid, QString sguid, QString payload)
{
    emit sigClientDataReceived(sid, sguid, payload);
}
