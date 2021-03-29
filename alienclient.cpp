#include "alienclient.h"

alienClient::alienClient(QObject *parent) : QObject(parent)
{

}


void alienClient::init()
{
    // set basic vars
    socketID = "";
    socketGUID = "";

    socketMode = 0;

    // create new client socket
    clientSocket = new QTcpSocket();

    // generate a socket-globally unique id (sguid) for this socket
    // ... 1 socket == 1 TCP or UDP connection
    // ... we use TCP in ALIEN
    //     ... for stability (sequential packet delivery)
    generateSGUID();

    connect(clientSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));
}

void alienClient::init(QTcpSocket *qsock)
{
    // initialize with an already-connected QTcpSocket from an ALIEN Server
    clientSocket = qsock;

    socketID = "";
    socketID = clientSocket->peerAddress().toString();

    socketMode = 0;

    generateSGUID();

    connect(clientSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));
}


void alienClient::generateSGUID()
{
    socketGUID = "";

    // create 256-byte buffer
    Botan::secure_vector<Botan::byte> rmsg(256);

    // use da botan random number generator (RNG) ta provide da input mon
    Botan::AutoSeeded_RNG rng;

    // generate random message in <rmsg>
    rng.randomize(&rmsg[0], rmsg.size());

    // # hashPipe:
    //   generates a hash product with input message <rmsg> using the Whirlpool hash algo
    Botan::Pipe hashPipe(new Botan::Hash_Filter("Whirlpool"), new Botan::Hex_Encoder);
    hashPipe.start_msg();
    hashPipe.write(rmsg);
    hashPipe.end_msg();

    // set sguid with product from <hashPipe>
    socketGUID = QString::fromStdString(hashPipe.read_all_as_string());
}


void alienClient::setSocketMode(int n_Mode)
{
    socketMode = n_Mode;
}


void alienClient::connectToDevice(QString deviceIP, quint16 openPort)
{
    socketID = clientSocket->peerAddress().toString();

    clientSocket->connectToHost(deviceIP, openPort);
}


void alienClient::zeroProtocol()
{
    socketID = "";
    socketGUID = "";

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
    // # lowDelayOption
    // 0 = enable | 1 = disable
    // Enables or disables the NAGLE alorithm
    //    ... NAGLE optimizes packet delivery by reducing latency
    //    ... useful for whatever the fuck; i have no idea what suffering this is supposed to eleviate, but whatever
    //    ... i keeps this bitch disabled
    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    // # SendBufferSizeSocketOption
    // Sets the size of outgoing packets [from this device]
    clientSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 32767);

    // # ReceiveBufferSizeSocketOption
    // Sets the size of incoming packets [to this device]
    clientSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 32767);

    socketID = clientSocket->peerAddress().toString();

    emit sigConnected(socketID, socketGUID);
}

void alienClient::onClientDisconnected()
{
    emit sigDisconnected(socketID, socketGUID);
}

void alienClient::onClientDataReceived()
{
    QByteArray buf = clientSocket->readAll();

    emit sigDataReceived(socketID, socketGUID, QString::fromUtf8(buf));
}
