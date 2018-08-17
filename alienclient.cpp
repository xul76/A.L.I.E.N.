/*
 *                       A.L.I.E.N. Server
 *
 *           a little interface (for) easy networking
 *
 *              https://github.com/xul76/A.L.I.E.N.
 */
#include "alienclient.h"

alienClient::alienClient(QObject *parent) : QObject(parent)
{
    // We don't use this constructor for one reason:
    //
    // In case a client connects to us, we'll already have an initialised socket when we don't need it.
    //
    // This means redundancy comes into play. Manual initialisation through the two init() functions is
    // a better option, because it allows devs to play into two different scenarios:
    //
    //    - when a.l.i.e.n. clients are used independently
    //    - when a.l.i.e.n. clients are used by a.l.i.e.n. servers
}

void alienClient::init()
{
    // set basic vars
    socketID = "";
    socketGUID = "";
    linkStatus = 0;

    // create new [client] socket
    clientSocket = new QTcpSocket();

    // generate and set sguid for this a.l.i.e.n. client
    generateSGUID();

    // connect the cS::connected() signal to our onClientConnected() function
    connect(clientSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    
    // connect the cS::disconnected() signal to our onClientDisconnected() function
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    
    // connect the cS::readyRead() signal to our onClientDataReceived() function
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));

    // disable nagles algorithm on the socket. in most situations, it only introduces lag lol.
    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 0);

    // set cS output buffer to 32767 bytes
    clientSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 32767);
    
    // set cS in buffer to 32767 bytes
    clientSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 32767);
}

void alienClient::init(QTcpSocket *qsock)
{
    // initialise cS with an already-connected QTcpSocket from an a.l.i.e.n. server
    clientSocket = qsock;

    // set basic vars
    socketID = "";
    socketID = clientSocket->peerAddress().toString();
    linkStatus = 0;

    generateSGUID();

    connect(clientSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));

    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    clientSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 32767);
    clientSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 32767);
}

void alienClient::generateSGUID()
{
    socketGUID = "";

    // create 256-byte buffer for random input
    Botan::secure_vector<Botan::byte> rmsg(256);

    // use a botan random number generator (RNG) to provide the input
    Botan::AutoSeeded_RNG rng;

    // generate random message in <rmsg>
    rng.randomize(&rmsg[0], rmsg.size());

    // # hashPipe:
    //   generates a hash product with input message <rmsg> using the TIGER hash algo.
    //   these hashpipes are convinience methods for generating hashes. there are better ways.
    Botan::Pipe hashPipe(new Botan::Hash_Filter("Tiger"), new Botan::Hex_Encoder);
    hashPipe.start_msg();
    // write <rmsg> to <hashPipe>
    hashPipe.write(rmsg);
    hashPipe.end_msg();

    // set sguid with product from <hashPipe>
    socketGUID = QString::fromStdString(hashPipe.read_all_as_string());
}

void alienClient::connectToDevice(QString deviceIP, int openPort)
{
    socketID = clientSocket->peerAddress().toString();

    clientSocket->connectToHost(deviceIP, openPort);
}

void alienClient::shutdown()
{
    if(linkStatus == 1)
    {
        clientSocket->disconnectFromHost();
    }
}

void alienClient::zeroProtocol()
{
    delete clientSocket;
}

void alienClient::sendData(QString payload)
{
    QByteArray buf = payload.toUtf8();
    clientSocket->write(buf);
}

void alienClient::onClientConnected()
{
    linkStatus = 1;

    if(socketID == "")
    {
        socketID = clientSocket->peerAddress().toString();
    }

    emit sigConnected(socketID, socketGUID);
}

void alienClient::onClientDisconnected()
{
    linkStatus = 0;
    emit sigDisconnected(socketID, socketGUID);
}

void alienClient::onClientDataReceived()
{
    // note: the qt documentation says to check for available bytes on the socket first. in my own experiences,
    // however, the situation arises where the socket sometimes reports 0 available bytes even though there are
    // > 0 bytes still to be read; hence this approach.
    QByteArray buf = clientSocket->readAll();

    emit sigDataReceived(socketID, socketGUID, QString::fromUtf8(buf));

}
