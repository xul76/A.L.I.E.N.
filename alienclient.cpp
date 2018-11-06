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
    socketID = "";
    socketGUID = "";

    linkStatus = 0;

    // create new [client] socket
    clientSocket = new QTcpSocket();

    // generate and set sguid for this a.l.i.e.n. client
    generateSGUID();

    // connect socket::connected() signal to onClientConnected() function
    connect(clientSocket,
            SIGNAL(connected()),
            this,
            SLOT(onClientConnected())
            );
    
    // connect socket::disconnected() signal to onClientDisconnected() function
    connect(clientSocket,
            SIGNAL(disconnected()),
            this,
            SLOT(onClientDisconnected())
            );
    
    // connect socket::readyRead() signal to onClientDataReceived() function
    connect(clientSocket,
            SIGNAL(readyRead()),
            this,
            SLOT(onClientDataReceived())
            );
}

void alienClient::init(QTcpSocket *qsock)
{
    clientSocket = qsock;

    socketID = "";
    socketID = clientSocket->peerAddress().toString();

    linkStatus = 0;

    // generate and set sguid for this a.l.i.e.n. client
    generateSGUID();
    
    // connect socket::connected() signal to onClientConnected() function
    connect(clientSocket,
            SIGNAL(connected()),
            this,
            SLOT(onClientConnected())
            );
    
    // connect socket::disconnected() signal to onClientDisconnected() function
    connect(clientSocket,
            SIGNAL(disconnected()),
            this,
            SLOT(onClientDisconnected())
            );
    
    // connect socket::readyRead() signal to onClientDataReceived() function
    connect(clientSocket,
            SIGNAL(readyRead()),
            this,
            SLOT(onClientDataReceived())
            );
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
    clientSocket->flush();
}

void alienClient::sendData(QByteArray _buf)
{
    clientSocket->write(_buf);
    clientSocket->flush();
}

void alienClient::onClientConnected()
{
    // 0 = enable nagle | 1 = disable nagle
    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    clientSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 131068);
    clientSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 131068);

    linkStatus = 1;

    socketID = clientSocket->peerAddress().toString();

    emit sigConnected(socketID, socketGUID);
}

void alienClient::onClientDisconnected()
{
    linkStatus = 0;

    emit sigDisconnected(socketID, socketGUID);
}

void alienClient::onClientDataReceived()
{
    QByteArray buf = clientSocket->readAll();

    emit sigDataReceived(socketID, socketGUID, QString::fromUtf8(buf));
}
