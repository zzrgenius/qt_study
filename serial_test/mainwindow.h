#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialprocess.h"
#include "settingconfig.h"
#include "tcphelper.h"

#include "ttkmarqueelabel.h"
#include "ui_mainwindow.h"
#include "utilities.h"
#include <QActionGroup>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QHostAddress>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMutex>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QPixmap>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QUuid>
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
  const QString ui_getSendData(void);

  void ui_setSendData(const QString &str);
  void ui_clearSendData(void);

  void serial_send(const QString &data, int len);
  void serial_send(void);
  SerialProcess *getSerialPtr();
  // Recieve
  int ui_recieve_getRecieveMode(void);

  bool ui_show_isEnableAutoNewLine(void);

  bool ui_show_isEnableShowSend(void);

  bool ui_show_isEnableShowTime(void);
  bool ui_isEnableBufferMode(void);
  bool ui_show_isEnableShowColor(void);
  int ui_recvieve_getBufferSize(void);
  // Send
  int ui_send_getSendMode(void);

  bool ui_send_isEnableAutoRepeat(void);
  void ui_send_setAutoRepeatState(bool set);

  int ui_send_getRepeatTime(void); // Unit: ms

  int ui_send_getRepeatTimeUnit(void);
  // LOG
  const QString ui_log_getLogPath();
  bool ui_log_isEnableLog(void);

public slots:

  void handle_serial_recieve_data(const QByteArray &data, int len);
  void handle_serial_error(int error);

private slots:
  void on_openButton_clicked();

  void on_clearRxButton_clicked();
  //  void serialPortWrite();

  void on_pbtSend_clicked();
  void handle_serialhelper_readyread(void);
  void on_clearTxHistoryButton_clicked();
  void handle_setting_changed(SettingConfig config);
  void on_cb_CRC_stateChanged(int arg1);

  void on_pteSend_textChanged();

  void on_tabWidget_tabBarClicked(int index);

  void on_tabWidget_currentChanged(int index);

  void on_openNetButton_clicked();

signals:
  void ui_serial_config_changed();

  void ui_serial_open(void);
  void ui_serial_close(void);
  void ui_serial_start(void);
  void ui_serial_pause(void);
  void ui_serial_send(const QByteArray &data, int len);

  void ui_tcp_start(const QString &ip, int port, int role);
  void ui_tcp_stop(int role);
  void ui_tcp_send(const QByteArray &data, int len, int role,
                   const QString &desip, int id = -1);

private:
  Ui::MainWindow *ui;
  QLabel *slabel;
  QLabel *lbTxBytes;
  QLabel *lbRxBytes;
  TTKMarqueeLabel *lbLogPath;

  QSerialPort *serialPort;
  SerialProcess *my_serial = nullptr; // 串口助手
  TCPHelper *tcp_helper = nullptr;
  //  UDPHelper *udp_helper = nullptr;
  QMutex m_mutex;
  QUuid myuuid;
  ulong txBytes = 0;
  ulong rxBytes = 0;
  QByteArray fileBuffer =
      QByteArray(settingConfig.logConfig.bufferSize * 1024, '\0');

  QByteArray recieveBuffer;

  int currentTab = 0;
  int serialStatus = STATUS_CLOSE;
  int networkStatus = STATUS_CLOSE;

  bool isAutorefresh = false;

  QTimer refresh_port_timer;
  QTimer resend_timer;
  SettingConfig settingConfig;

  int ui_getCurrentTab(void);

  void ui_log_setLogPath(const QString &p);

  void ui_log_setSaveLogState(bool save);

  void ui_log_logSaveToFile(const QString &str);
  void ui_log_seleteLogPath(void);

  //扫描串口
  void scanSerialPort();
  void ui_initSetting(SettingConfig *config);

  void ui_initConnect(void);
  void ui_creatToolBar(void);

  void ui_creatStatusBar(void);

  void apply_ui_serial_config(const SerialConfig &config);

  void update_ui_serial_config(void);

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

  void ui_serial_toggle_pbtSend(bool _isOpen);

  const QString inline ui_net_makeInterfaceStr(QNetworkInterface interface) {
    //         if(interface.isValid()) return QString("");
    const QString str = QString::fromUtf8("%1 [%2]")
                            .arg(interface.humanReadableName())
                            .arg(interface.hardwareAddress());
    return str;
  }

  void tf_handle_tick();
  void ui_net_addInterface(QNetworkInterface interface);
  const QString ui_net_getCurrentInterfaceHardAddr(void);

  const QString ui_net_getCurrentInterfaceHumanNamme(void);

  const QString ui_net_getCurrentInterfaceAddr(bool ipv6 = false);

  void ui_net_setCurrentInterface(QNetworkInterface interface);
  QNetworkInterface ui_net_getInterface(bool *ok);

  bool ui_net_isEnableIPV6(void);

  int ui_net_getRole(void);

  int ui_net_getProfile(void);

  int ui_net_getPort(void);

  const QString ui_net_getIP(void);

  void ui_net_setIP(const QString &ip);
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

  QColor ui_recieve_getRecieveFontColor(void);
  void ui_recieve_setRecieveFontColorState(bool state);

  void ui_setShowPlaintFont(const QFont &font);
};
#endif // MAINWINDOW_H
