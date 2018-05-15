/*
 *                       A.L.I.E.N. Client
 *
 *  a little interface (for) easy networking (client component)
 *
 *              https://github.com/xul76/A.L.I.E.N.
 */

#ifndef ALIENCLIENT_H
#define ALIENCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QByteArray>
#include <QString>
#include <QStringList>
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
 *  some cpp witchcraft
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
    //   the socket used to establish a connection to a local/remote app or device and to exchange
    //   data with each other.
    QTcpSocket *clientSocket;

    // # socketID:
    //   a.l.i.e.n. client: IPv4 Address of the server ([server] endpoint)
    //   a.l.i.e.n. server: IPv4 Address of the alienClient object ([client] endpoint)
    QString socketID;

    // # socketGUID:
    //   used for NAT purposes. see attached documentation.
    QString socketGUID;

    // # connectionAlive:
    //   a bit obvious (get it?)
    bool connectionAlive;

signals:
    // # sigClientConnected:
    //   emitted by this class when it successfully connects to an endpoint (local/remote app or device).
    void sigClientConnected(QString sid, QString sguid);

    // # sigClientDisconnected:
    //   emitted by this class when it disconnects (or is disconnected) from the endpoint.
    void sigClientDisconnected(QString sid, QString sguid);

    // # sigClientDataReceived:
    //   emitted by this class when the endpoint sends data to cS/your app.
    void sigClientDataReceived(QString sid, QString sguid, QString payload);

public slots:
    // # init:
    //   !! must be called right after creating a new alienClient object. !!
    void init();

    // # init (overloaded):
    //   !! used by an alienServer object only. should never be called by devs. !!
    void init(QTcpSocket *qsock);

    // # generateGUID:
    //   generates a new sguid (Socket Globally-Unique IDentifier). see attached documentation.
    void generateGUID();

    // # connectToServer:
    //   initiates a new TCP/IP connection to a server (app socket in listening mode on a local or
    //   remote device) on <serverIP> on port <serverPort>.
    void connectToServer(QString serverIP, int serverPort);

    // # shutdown:
    //   closes an existing connection and cleans up.
    void shutdown();

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
