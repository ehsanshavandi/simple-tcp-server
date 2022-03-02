#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QRegExpValidator>
#include <QDebug>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  const QHostAddress& getIp() const;

  quint16 getPort() const;


 private slots:
  void on_btnStart_clicked();
  void on_btnStop_clicked();
  void on_btnClose_clicked();

  void on_comboBox_currentTextChanged(const QString& arg1);
  void newClient();
  void readyReadData();
  void disconnectClient();

  // Helper method
  QHostAddress createIpAddr();
  quint16 createPort();
  void startServer(bool& ok);
  void stopServer();
  void clearServer();
  void clearSockets();

 private:
  Ui::MainWindow* ui;
  QTcpServer* server;
  quint16 port;
  QValidator* portValidator;
  QHostAddress ip;
  QStringList ipList;
  QList<QTcpSocket*> clientList;
  bool isListening;
  void init();
};
#endif // MAINWINDOW_H
