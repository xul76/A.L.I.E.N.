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

bool alienServer::startServer(int localPort)
{
    // # Your homework
    //
    //   + obtain possible error codes/messages
    //      - return this information as a proper error instead of a bool
    //
    //   - select one appropriate network interface to listen on instead of all available interfaces
    //
    //   - use even less code in this function

    // # opRes
    //   true:  sS is now successfully listening on port <localPort>.
    //   false: sS could not be started with these parameters.
    bool opres = false;

    // call sS::listen(), which causes sS/your app to listen on port <localPort> on all available
    // network interfaces. for most cases, this code is perfectly fine, but devs seeking to control
    // on which network interface the app should be accessible should do the homework.
    opres = serverSocket->listen(QHostAddress::Any, localPort);

    return opres;
}

void alienServer::stopServer()
{
    // close sS, which disconnects all connected devices and prevents further connections to sS/your app.
    serverSocket->close();

    // loop through C
    for(int i = 0; i < controller.count(); i++)
    {
        // call the shutdown() function on each a.l.i.e.n. client
        controller.at(i)->shutdown();
    }
    
    // reset C
    controller.clear();
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
            SIGNAL(sigClientDisconnected(QString, QString)),
            this,
            SLOT(serverDisconnected(QString, QString))
            );

    // connect the nsock::sigClientDataReceived() signal to our serverDataReceived() function. a nsock
    // emits this signal when this endpoint sends data to your app.
    connect(nsock,
            SIGNAL(sigClientDataReceived(QString, QString, QString)),
            this,
            SLOT(serverDataReceived(QString, QString, QString))
            );

    // add nsock to C, so that it can be accessed/managed as needed
    controller.append(nsock);

    emit sigClientConnected(nsock->socketID, nsock->socketGUID);
}

void alienServer::serverDisconnected(QString sid, QString sguid)
{
    // loop through C
    for(int i = 0; i < controller.count(); i++)
    {
        // find a.l.i.e.n. client tied to this endpoint by <sid> and <sguid>
        if (controller[i]->socketID == sid && controller[i]->socketGUID == sguid)
        {
            // delete this disconnected client's a.l.i.e.n. client object
            delete controller[i];
            // remove the entry from C
            controller.removeAt(i);
        }
    }
    emit sigClientDisconnected(sid, sguid);
}

void alienServer::serverDataReceived(QString sid, QString sguid, QString payload)
{
    emit sigClientDataReceived(sid, sguid, payload);
}
