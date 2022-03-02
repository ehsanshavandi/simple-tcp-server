#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  init();
}

MainWindow::~MainWindow()
{
  this->clearSockets();
  this->clearServer();

  delete ui;
}


void MainWindow::on_btnStart_clicked()
{
  this->ip = this->createIpAddr();
  this->port = this->createPort();

  qInfo() << this->ip;
  qInfo() << this->port;
  bool isServerUpOk;
  this->startServer(isServerUpOk);

  if (!isServerUpOk)
    QMessageBox::information(this, "Error", "server could not start");

}


void MainWindow::on_btnStop_clicked()
{
  this->clearSockets();
  this->stopServer();
}


void MainWindow::on_btnClose_clicked()
{
  QApplication::quit();
}

quint16 MainWindow::getPort() const
{
  return port;
}

const QHostAddress& MainWindow::getIp() const
{
  return ip;
}

void MainWindow::init()
{
  QList<QHostAddress> interfaces = QNetworkInterface::allAddresses();
  QHostAddress interface;

  foreach (interface, interfaces)
  {
    ipList.append(interface.toString());
  }

  ui->comboBox->insertItems(0, ipList);
  server = new QTcpServer(this);
  this->isListening = false;

  if (isListening)
  {
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
  }
  else
  {
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
  }
}


void MainWindow::on_comboBox_currentTextChanged(const QString& arg1)
{

}

void MainWindow::newClient()
{
  QTcpSocket* socket = server->nextPendingConnection();
  clientList.append(socket);
  connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readyReadData);
  connect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);

  // broadcast the message
  foreach (QTcpSocket* client, clientList)
  {
    if (client != socket)
      client->write(QByteArray::fromStdString(socket->peerAddress().toString().toStdString() +
                                              " connected to server !\n"));
  }
}

void MainWindow::readyReadData()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  QByteArray datas = socket->readAll();

  // broadcast the message
  foreach (QTcpSocket* client, clientList)
  {
    if (client != socket)
      client->write(QByteArray::fromStdString(socket->peerAddress().toString().toStdString() + ": " +
                                              datas.toStdString()));
  }
}

void MainWindow::disconnectClient()
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  clientList.removeOne(socket);
}

QHostAddress MainWindow::createIpAddr()
{
  QString selectedIp = ui->comboBox->currentText();
  QHostAddress ipAddr = QHostAddress(selectedIp);
  return ipAddr;
}

quint16 MainWindow::createPort()
{
  portValidator = new QRegExpValidator(
    QRegExp("^(102[0-3]|10[0-1]\d|[1-9][0-9]{0,2}|0)$"),
    this);
  ui->lineEditPort->setValidator(portValidator);
  bool isPortOk;
  auto portStr = ui->lineEditPort->text();
  port = portStr.toDouble(&isPortOk);

  if (!isPortOk)
  {
    qInfo() << "error";
    return -1;
  }
  else
    return port;
}

void MainWindow::startServer(bool& ok)
{
  ok = server->listen(this->ip, this->port);

  if (!ok)
    qInfo() << "server could not start";
  else
  {
    connect(server, &QTcpServer::newConnection, this, &MainWindow::newClient);
    this->isListening = true;
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->lblStatus->setText("Server is listening...");
  }
}

void MainWindow::stopServer()
{
  server->close();
  this->isListening = false;
  ui->btnStart->setEnabled(true);
  ui->btnStop->setEnabled(false);
  ui->lblStatus->setText("Server is off");
}

void MainWindow::clearServer()
{
  server->close();
  server->deleteLater();
}

void MainWindow::clearSockets()
{
  foreach (QTcpSocket* socket, clientList)
  {
    socket->close();
    socket->deleteLater();
  }
}


