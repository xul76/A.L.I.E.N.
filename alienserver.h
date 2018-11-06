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

#include "alienclient.h"

class alienServer : public QObject
{
    Q_OBJECT
public:
    explicit alienServer(QObject *parent = nullptr);

    // # serverSocket:
    //   a socket which will listen (wait) for incoming TCP/IP connections.
    QTcpServer *serverSocket;

    // # controller:
    //   list containing [connected] a.l.i.e.n clients.
    QList<alienClient *> controller;

    // # adapterIP:
    //   LAN IP address of this machine.
    QString adapterIP;

signals:
    // # sigClientConnected:
    //   when a remote app (client) connects to this app.
    void sigClientConnected(QString sid, QString sguid);

    // # sigClientDisconnected:
    //   when a client disconnects from this app, or when you disconnect them server-side.
    void sigClientDisconnected(QString sid, QString sguid);

    // # sigClientDataReceived:
    //   when our server socket receives data from a remote device.
    void sigClientDataReceived(QString sid, QString sguid, QString payload);

public slots:
    // # startServer:
    //   set server socket to listen on port <localPort> and begin accepting network/Internet connections.
    bool startServer(QHostAddress address, int localPort);

    // # stopServer:
    //   shuts down our server socket and prevent further incoming connections from being established to your app.
    void stopServer();

    // # getNetworkInterfaces:
    //   returns a list of available network interfaces and their IP Addresses.
    QStringList getNetworkInterfaces();

    // # zeroProtocol:
    //   shut down and clean up.
    void zeroProtocol();

    // # sendData:
    //   sends <payload> to endpoint<sid,sguid>.
    void sendData(QString sid, QString sguid, QString payload);

private slots:
    // # serverConnected:
    //   fired on client connection establishment.
    void serverConnected();
    
    // # serverDisconnected:
    //   fired on client disconnection.
    void serverDisconnected(QString sid, QString sguid);
    
    // # serverDataReceived:
    //   fired on client data arrival.
    void serverDataReceived(QString sid, QString sguid, QString payload);
};

#endif // alienServer_H
