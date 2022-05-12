#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTableWidgetItem>

#include "serialprocess.h"
#include "settingconfig.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  int btn_on_off = 0;
  enum { TAB_SERIAL = 0, TAB_NETWORK = 1 };
  enum DEVICE_STATUS {
    STATUS_CLOSE = -1,
    STATUS_OPEN = 0,
    STATUS_PAUSE = 1,
    STATUS_OCCUR_ERROR = 2
  };

  void serial_send(const QString &data, int len);
  void serial_send(void);

 public slots:

  void handle_serial_recieve_data(const QByteArray &data, int len);
  void handle_serial_error(int error);

 private slots:
  void on_openButton_clicked();
  void SerialPortReadyRead_slot();

  void on_clearRxButton_clicked();
  void serialPortWrite();

  void on_SaveRxBufButton_clicked();

  void on_pbtSend_clicked();
  void handle_serialhelper_readyread(void);

 private:
  Ui::MainWindow *ui;
  QLabel *slabel;
  QLabel *lbTxBytes;
  QLabel *lbRxBytes;
  QSerialPort *serialPort;
  SerialProcess *my_serial = nullptr;  // 串口助手
                                       //  TCPHelper *tcp_helper = nullptr;
                                       //  UDPHelper *udp_helper = nullptr;
  QMutex m_mutex;
  ulong txBytes = 0;
  ulong rxBytes = 0;
  QByteArray recieveBuffer;

  int currentTab = 0;
  int serialStatus = STATUS_CLOSE;

  bool isAutorefresh = false;

  QTimer refresh_port_timer;
  QTimer resend_timer;
  SettingConfig settingConfig;

  int ui_getCurrentTab(void);

  //扫描串口
  void scanSerialPort();
  void ui_initSetting(SettingConfig *config);

  void ui_initConnect(void);
  void ui_creatToolBar(void);

  void ui_creatStatusBar(void);

  void ui_serial_setPortName(QSerialPortInfo info);
  void ui_serial_setBaud(qint64 baud);
  void ui_serial_setDataBit(QSerialPort::DataBits d);
  void ui_serial_setParity(QSerialPort::Parity p);
  void ui_serial_setStopBit(QSerialPort::StopBits s);
  void ui_serial_setFlow(QSerialPort::FlowControl f);
  int ui_serial_getPortNumber(void);
  const QString ui_serial_getPortName(void);
  qint64 ui_serial_getBaud(void);
  QSerialPort::DataBits ui_serial_getDataBit(void);
  QSerialPort::Parity ui_serial_getParity(void);

  QSerialPort::StopBits ui_serial_getStopBit(void);
  QSerialPort::FlowControl ui_serial_getFlow(void);

  const SerialConfig ui_serial_getConfig(void);

  void ui_serial_setConfig(SerialConfig config);
  void ui_serial_toggle_pbtSend(bool _isOpen);

  const QString inline ui_net_makeInterfaceStr(QNetworkInterface interface) {
    //         if(interface.isValid()) return QString("");
    const QString str = QString::fromUtf8("%1 [%2]")
                            .arg(interface.humanReadableName())
                            .arg(interface.hardwareAddress());
    return str;
  }
  void ui_net_addInterface(QNetworkInterface interface);
  const QString ui_net_getCurrentInterfaceHardAddr(void);

  const QString ui_net_getCurrentInterfaceHumanNamme(void);

  const QString ui_net_getCurrentInterfaceAddr(bool ipv6 = false);

  void ui_net_setCurrentInterface(QNetworkInterface interface);
  QNetworkInterface ui_net_getInterface(bool *ok);

  void ui_showTime();
  void ui_showSend(const QString &str, bool t = false);
  void ui_showRecieve(const QString &str, bool t = false);
  void ui_clearRecieve(void);
  void ui_showRecieveData(const QByteArray &data, int len);
  //    void ui_showSendData(const QByteArray& data, int len);
  void ui_showMessage(const QString &message, int time = 0,
                      QColor color = Qt::darkGreen);
  void ui_statusbar_showMessage(const QString &str,
                                QColor color = Qt::darkGreen);
  void ui_statusbar_showRxBytes(ulong bytes);
  void ui_statusbar_showTxBytes(ulong bytes);
  void ui_statusbar_showLogPath(const QString &str);
  void ui_addSendHistory(const QString &str);
  void ui_addSendHistory(const QStringList &list);
  void ui_clearSendHistory(void);

  void ui_recieve_initRecieveFontColor(void);
};
#endif  // MAINWINDOW_H
