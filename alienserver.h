/*
 *               A.L.I.E.N Server
 *
 *  A Little Interface (for) Easy Networking
 *
 *              (server component)
 *
 *     https://github.com/xul76/A.L.I.E.N
 */

#ifndef ALIENSERVER_H
#define ALIENSERVER_H

#include <QObject>
#include <QtNetwork>

#include <iostream>
#include <exception>

#include "alienclient.h"

class alienServer : public QObject
{
    Q_OBJECT
public:
    explicit alienServer(QObject *parent = 0);

    // # serverSocket
    //   Listens for incoming connection requests
    //      ... ALIEN uses port 51000 by default
    QTcpServer *serverSocket;

    // # controller
    //   List of connected devices (managed as ALIEN clients)
    QList<alienClient *> controller;

    // # adapterIP
    //   Our server's IP (non-WAN)
    QString adapterIP;

signals:
    // # sigClientConnected
    //   Emitted by ALIEN server when a program or device connects to your serverSocket
    void sigClientConnected(QString sid, QString sguid);

    // # sigClientDisonnected
    //   Emitted by ALIEN server when a connected device disconnects from your serverSocket
    void sigClientDisconnected(QString sid, QString sguid);

    // # sigClientDisonnected
    //   Emitted by ALIEN server when a connected device sends data to your serverSocket
    void sigClientDataReceived(QString sid, QString sguid, QString payload);

    // # sigServerError
    //   Take a guess Einstein
    void sigServerError(QString msg);

public slots:
    // # startServer
    //   Begins accepting incoming connection requests from local- and remote programs/devices
    bool startServer(QHostAddress address, quint16 localPort);

    // # stopServer
    //   Shuts down our server socket
    //      ... prevents any further connection requests from being evaluated
    //             ... those are dropped immediately after this function is called
    void stopServer();

    // # getNetworkInterfaces
    //   Returns a QStringList similair to Linux's "ip addr" or Windowzez "ipconfig" list of
    //   availables network interfaces/adapters and their IP addresses
    QStringList getNetworkInterfaces();

    // # zeroProtocol
    //   Kill all active connections and destroy ALIEN client objects
    void zeroProtocol();

    // # sendData
    //   Send <payload> to endpoint@socketID + socketGUID
    void sendData(QString sid, QString sguid, QString payload);

private slots:
    // These signals are emitted to signal the main-thread (usually GUI) for briefing/log/debug purposes
    //    ... its better to handle complex tasks on the ALIEN client objects themselves (rather than the GU1I)
    void serverConnected();
    void serverDisconnected(QString sid, QString sguid);
    void serverDataReceived(QString sid, QString sguid, QString payload);

};

#endif // alienServer_H
