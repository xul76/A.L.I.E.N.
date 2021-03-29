/*
 *               A.L.I.E.N Client
 *
 *  A Little Interface (for) Easy Networking
 *
 *              (client component)
 *
 *     https://github.com/xul76/A.L.I.E.N
 */

#ifndef ALIENCLIENT_H
#define ALIENCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>

#include "botan/pipe.h"
#include "botan/hex.h"
#include "botan/filters.h"
#include "botan/aes.h"
#include "botan/pbkdf2.h"
#include "botan/hmac.h"
#include "botan/sha160.h"
#include "botan/secmem.h"
#include "botan/tiger.h"
#include "botan/keccak.h"
#include "botan/auto_rng.h"

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

    QTcpSocket *clientSocket;

    // # socketID:
    //   IP4 Address of the other dude (endpoint)
    QString socketID;

    // # socketGUID:
    //   Assign each socket you create a unique id, so that incoming/outgoing communications with that endpoint are
    //   easy to handle. (see dox)
    QString socketGUID;

    // # socketMode
    //   0 = Text Mode
    //       ... in Text Mode the client sends + receives commands to/from the server
    //   1 = Binary Mode
    //       ... in Binary Mode the socket is receiving a file, and is therefore not processing [server] commands
    int socketMode;

    // # bytesReceived
    //   Tracks received bytes from endpoint [during file transfer]
    int bytesReceived;

    // # totalBytes
    //   Total file size (in bytes) of incoming file
    long totalBytes;

    // # outputFile
    //   Target file on disk for storing the incoming file
    QString outputFile;

    // # outputFile
    //   File name of [future] target file on disk
    QString outputFileName;

    // # inputFile
    //   Target file on disk that's to be uploaded next
    QString inputFile;

    // # writer
    //   File handler for incoming file transfers
    QFile writer;

signals:
    void sigConnected(QString sid, QString sguid);
    void sigDisconnected(QString sid, QString sguid);
    void sigDataReceived(QString sid, QString sguid, QString payload);

public slots:
    void init();
    void init(QTcpSocket *qsock);

    void generateSGUID();

    void setSocketMode(int n_Mode);

    void connectToDevice(QString deviceIP, quint16 openPort);

    void zeroProtocol();

    void sendData(QString payload);
    void sendData(QByteArray _buf);

private slots:
    void onClientConnected();
    void onClientDisconnected();
    void onClientDataReceived();

};

#endif // ALIENCLIENT_H
