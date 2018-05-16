/*
 *                       A.L.I.E.N. Server
 *
 *  a little interface (for) easy networking (server component)
 *
 *              https://github.com/xul76/A.L.I.E.N.
 */

#ifndef ALIENSERVER_H
#define ALIENSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QByteArray>
#include <QList>
#include <QString>
#include "alienclient.h"

class alienServer : public QObject
{
    Q_OBJECT
public:
    explicit alienServer(QObject *parent = 0);

    // # serverSocket: sS
    //   a [new] socket which will listen for incoming TCP/IP connections.
    //   accepts local (LAN) and remote (WAN) connections on a user-defined port.
    QTcpServer *serverSocket;

    // # controller: C
    //   sS produces an alienClient object upon establishing a new connection.
    //   these objects are stored in this list, so that they can be accessed for whatever reason.
    //   an essential feature of multi-client based apps, implemented very simply here.
    QList<alienClient *> controller;

signals:
    // # sigClientConnected:
    //   emitted by this class when a new client (local/remote device) connects to your app.
    void sigClientConnected(QString sid, QString sguid);

    // # sigClientDisconnected:
    //   emitted by this class when a client disconnects from your app, or when you disconnect it
    //   server-side.
    void sigClientDisconnected(QString sid, QString sguid);

    // # sigClientDataReceived:
    //   emitted by this class when a client sends data to sS/your app.
    void sigClientDataReceived(QString sid, QString sguid, QString payload);

public slots:
    // # startServer:
    //   creates a new socket on port <localPort> and begins accepting local/remote connections from
    //   apps and devices. After this function returns, local/remote apps and devices can begin
    //   connecting to your app.
    bool startServer(int localPort);
    
    // # stopServer:
    //   shuts down our server socket, cleans up and prevents further incoming connections from being
    //   established to sS/your app.
    void stopServer();

    // # sendData:
    //   sends <payload> to endpoint<sid,sguid>.
    void sendData(QString sid, QString sguid, QString payload);

private slots:
    // # serverConnected:
    //   associated with an a.l.i.e.n. client::sigClientConnected() signal. Emitted when a connection is
    //   established to your app with an endpoint.
    void serverConnected();
    
    // # serverDisconnected:
    //   associated with an a.l.i.e.n. client::sigClientDisconnected() signal. Emitted when the connection
    //   to this endpoint is lost.
    void serverDisconnected(QString sid, QString sguid);
    
    // # serverDataReceived:
    //   associated with an a.l.i.e.n. client::sigClientDataReceived() signal. Emitted when new data
    //   arrives on our app from this endpoint.
    void serverDataReceived(QString sid, QString sguid, QString payload);
};

#endif // alienServer_H
