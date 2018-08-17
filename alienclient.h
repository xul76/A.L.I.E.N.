/*
 *                       A.L.I.E.N. Server
 *
 *           a little interface (for) easy networking
 *
 *              https://github.com/xul76/A.L.I.E.N.
 */
#ifndef ALIENCLIENT_H
#define ALIENCLIENT_H

#include <QObject>
#include <QtNetwork>

/*
 *  Botan 2 Cryptographic Library Headers
 */
#include "botan/botan.h"
#include "botan/pipe.h"
#include "botan/hex.h"
#include "botan/filters.h"
#include "botan/aes.h"
#include "botan/pbkdf2.h"
#include "botan/hmac.h"
#include "botan/sha160.h"
#include "botan/secmem.h"
#include "botan/tiger.h"
/*
 *  Botan-related witchcraft
 */
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include <stdio.h>

class alienClient : public QObject
{
    Q_OBJECT
public:
    explicit alienClient(QObject *parent = 0);

    // # clientSocket: cS
    //   the socket used to establish a connection to another app or device and to exchange data with each other.
    QTcpSocket *clientSocket;

    // # socketID:
    //   a.l.i.e.n. client: IP4 Address of the server (endpoint)
    //   a.l.i.e.n. server: IP4 Address of a client (source)
    QString socketID;

    // # socketGUID:
    //   used for NAT purposes. see documentation.
    QString socketGUID;

    // # connectionAlive: aka an obvious bit
    //   false: offline
    //   true:  online
    bool linkStatus;

signals:
    // # sigClientConnected:
    //   emitted by this class when it successfully connects to an endpoint.
    void sigConnected(QString sid, QString sguid);

    // # sigClientDisconnected:
    //   emitted by this class when it disconnects (or is disconnected) from the endpoint.
    void sigDisconnected(QString sid, QString sguid);

    // # sigClientDataReceived:
    //   emitted by this class when the endpoint sends [a block of] data to cS/your app.
    void sigDataReceived(QString sid, QString sguid, QString payload);

public slots:
    // # init:
    //   !! must be called right after manually creating a new alienClient object. !!
    void init();

    // # init (overloaded):
    //   !! used by an alienServer object only. should never be called by devs. !!
    void init(QTcpSocket *qsock);

    // # generateGUID:
    //   generates a new sguid (Socket Globally-Unique IDentifier). see documentation.
    void generateSGUID();

    // # connectToServer:
    //   initiates a new TCP/IP connection to a server (app socket in listening mode on a local or
    //   remote device) on <serverIP> on port <serverPort>.
    void connectToDevice(QString deviceIP, int openPort);

    // # shutdown:
    //   closes the active connection.
    void shutdown();

    // # zeroProtocol:
    //   destroy everything (clean up).
    void zeroProtocol();

    // # sendData:
    //   sends <payload> to the endpoint.
    void sendData(QString payload);

private slots:
    // obvious
    void onClientConnected();

    // obvious
    void onClientDisconnected();

    // obvious
    void onClientDataReceived();
};

#endif // ALIENCLIENT_H
