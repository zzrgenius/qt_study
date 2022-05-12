#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTime>

#include "serialprocess.h"
#include "ui_mainwindow.h"
static int dataTotalRxCnt = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  const QMutexLocker locker(&m_mutex);
  this->setWindowTitle("DevilTools");

  ui_creatToolBar();
  ui_creatStatusBar();

  refresh_port_timer.start(1000);

  serialPort = new QSerialPort(this);
  my_serial = new SerialProcess(this);
  scanSerialPort();
  /* 接收数据信号槽 */
  connect(serialPort, &QSerialPort::readyRead, this,
          &MainWindow::SerialPortReadyRead_slot);
  QMessageBox::information(this, "Welcome", "哈哈哈哈");
  ui_initConnect();
}

MainWindow::~MainWindow() { delete ui; }
void MainWindow::ui_initConnect() {
  QObject::connect(my_serial, &SerialProcess::errorOccurred, this,
                   &MainWindow::handle_serial_error);
  QMetaObject::Connection s_connect = my_serial->callOnReadyRead(
      this, &MainWindow::handle_serialhelper_readyread);

  QObject::connect(&refresh_port_timer, &QTimer::timeout, [=]() {
    isAutorefresh = true;
    if (ui_getCurrentTab() == TAB_SERIAL) {          // 串口
                                                     //      scanSerialPort();
    } else if (ui_getCurrentTab() == TAB_NETWORK) {  // TCP/UDP
      //            ui_refreshNetInterface();
      QList<QNetworkInterface> nlist = QNetworkInterface::allInterfaces();
      int ls = nlist.size();
      if (ls == 0) return;
      int ns = this->ui->cbbNetworkInterface->count();

      if (ls != ns) {
        int ci = this->ui->cbbNetworkInterface->currentIndex();
        int inser = 0;
        int find = -1;
        QHostAddress addr;
        QNetworkInterface interface;
        int eth = -1;
        const QString chaddr = ui_net_getCurrentInterfaceHardAddr();
        this->ui->cbbNetworkInterface->clear();
        for (int i = 0; i < ls; i++) {
          interface = nlist.at(i);
          if (ns && (chaddr == interface.hardwareAddress())) {
            find = i;
            if (find > ci) {
              this->ui->cbbNetworkInterface->insertItem(
                  ci, ui_net_makeInterfaceStr(interface));
              continue;
            }
            if (find < ci) {
              inser = (ci < ls) ? (ci - find) : (ls - find);
            }
          }
          if (inser != 0) {
            inser -= 1;
            this->ui->cbbNetworkInterface->insertItem(
                find, ui_net_makeInterfaceStr(interface));
            continue;
          }
          ui_net_addInterface(interface);
          if ((eth < 0) && interface.type() == QNetworkInterface::Ethernet) {
            const QString nstr = interface.humanReadableName();
            if (nstr.contains(QString::fromUtf8("以太网")) ||
                nstr.contains(QString::fromUtf8("有线"))) {
              eth = i;
            }
          }
        }
        isAutorefresh = false;
        if (ci > ls - 1) ci = ls - 1;

        if (eth < 0)
          this->ui->cbbNetworkInterface->setCurrentIndex((ci < 0) ? 0 : ci);
        else {
          this->ui->cbbNetworkInterface->setCurrentIndex(eth);
        }
        //                return;
      }
    }
    isAutorefresh = false;
  });

  QObject::connect(this->ui->tabWidget, &QTabWidget::currentChanged,
                   [=](int index) {
                     currentTab = index;
                     if (index == 0) {  // serial
                       if (!my_serial) {
                         my_serial = new SerialProcess(this);
                       } else {
                         my_serial->close();
                       }
                     }
                     if (index == 1) {  // tcp/udp
                       if (my_serial) {
                         my_serial->close();
                       }
                     }
                   });
}
void MainWindow::scanSerialPort() {
  QStringList serialNamePort;

  /* 搜索所有可用串口 */
  foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()) {
    serialNamePort << inf0.portName();
  }
  ui->cbbPort->addItems(serialNamePort);
}
void MainWindow::on_openButton_clicked() {
  if (ui->openButton->text() == "打开串口") {
    /* 串口设置 */
    //    serialPort->setPortName(ui->cbbPort->currentText());
    //    serialPort->setBaudRate(ui->cbbBaud->currentText().toInt());
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    /* 打开串口提示框 */
    if (true == serialPort->open(QIODevice::ReadWrite)) {
      QMessageBox::information(this, "提示", "串口打开成功");
      //      ui->openButton->setEnabled(false);
      ui->cbbPort->setEnabled(false);
      ui->cbbBaud->setEnabled(false);
      ui->openButton->setText(tr("关闭串口"));

    } else {
      QMessageBox::critical(this, "提示", "串口打开失败");
    }
  } else if (ui->openButton->text() == "关闭串口") {
    ui->openButton->setText(tr("打开串口"));
    serialPort->close();
    //    ui->cbbPort->setEnabled(true);
    //    ui->cbbBaud->setEnabled(true);
  }
}

// void MainWindow::on_closeButton_clicked() {
//   serialPort->close();
//   ui->openButton->setEnabled(true);
// }

/*
    函   数：SerialPortReadyRead_slot
    描   述：readyRead()信号对应的数据接收槽函数
    输   入：无
    输   出：无
*/
void MainWindow::SerialPortReadyRead_slot() {
  //    QTime *time = new QTime();
  QString framedata;
  /*读取串口收到的数据*/
  QByteArray bytedata = serialPort->readAll();

  //  /*数据是否为空*/
  //  if (!bytedata.isEmpty()) {
  //    if (ui->HexDispcheckBox->isChecked()) {
  //      /*hex显示*/
  //      framedata = bytedata.toHex(' ').trimmed().toUpper();
  //      ui->readtextEdit->setTextColor(QColor(Qt::green));
  //    } else {
  //      /*ascii显示*/
  //      framedata = QString(bytedata);
  //      ui->readtextEdit->setTextColor(QColor(Qt::magenta));
  //    }
  //    if (ui->TimeDispcheckBox->isChecked()) {
  //      /*是否显示时间戳*/

  //      framedata = QString("[%1]:RX -> %2")
  //                      .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
  //                      .arg(framedata);
  //      ui->readtextEdit->append(framedata);
  //    } else {
  //      ui->readtextEdit->insertPlainText(framedata);
  //    }
  //    /*更新接收计数*/
  //    dataTotalRxCnt += bytedata.length();
  //    ui->rxCnt_label->setText(QString::number(dataTotalRxCnt));
  //  }
}
int MainWindow::ui_getCurrentTab() {
  return this->ui->tabWidget->currentIndex();
}
void MainWindow::on_clearRxButton_clicked() {
  //  ui->readtextEdit->clear();
  //  ui->rxCnt_label->setText(QString::number(0));
  dataTotalRxCnt = 0;
}
void MainWindow::serialPortWrite() {
  QByteArray SendTextEditBa;
  QString SendTextEditStr;
  //获取发送框字符

  //  SendTextEditStr = ui->sendtextEdit->document()->toPlainText();
  //判断是否非空
  if (SendTextEditStr.isEmpty()) {
    return;
  }
  if (ui->rbtRHex->isChecked()) {
    char ch;
    bool flag = false;
    uint32_t i, len;
    //去掉无用符号
    SendTextEditStr = SendTextEditStr.replace(' ', "");
    SendTextEditStr = SendTextEditStr.replace(',', "");
    SendTextEditStr = SendTextEditStr.replace('\r', "");
    SendTextEditStr = SendTextEditStr.replace('\n', "");
    SendTextEditStr = SendTextEditStr.replace('\t', "");
    SendTextEditStr = SendTextEditStr.replace("0x", "");
    SendTextEditStr = SendTextEditStr.replace("0X", "");
    //判断数据合法性
    //判断数据合法性
    for (i = 0, len = SendTextEditStr.length(); i < len; i++) {
      ch = SendTextEditStr.at(i).toLatin1();
      if (ch >= '0' && ch <= '9') {
        flag = false;
      } else if (ch >= 'a' && ch <= 'f') {
        flag = false;
      } else if (ch >= 'A' && ch <= 'F') {
        flag = false;
      } else {
        flag = true;
      }
    }
    if (flag) QMessageBox::warning(this, "警告", "输入内容包含非法16进制字符");
    SendTextEditBa = SendTextEditStr.toUtf8();
    serialPort->write(SendTextEditBa.fromHex(SendTextEditBa));
  } else {
    SendTextEditBa = SendTextEditStr.toUtf8();
    serialPort->write(SendTextEditBa);
  }
}
/*
    函   数：on_SaveData_Button_clicked
    描   述：保存数据按钮点击槽函数
    输   入：无
    输   出：无
*/

void MainWindow::on_SaveRxBufButton_clicked() {
  //  //  QString data = ui->readtextEdit->toPlainText();

  //  //  if (data.isEmpty()) {
  //  //    QMessageBox::information(this, "提示", "数据内容空");
  //  //    return;
  //  //  }

  //  QString curPath = QDir::currentPath();              //获取系统当前目录
  //  QString dlgTitle = "保存文件";                      //对话框标题
  //  QString filter = "文本文件(*.txt);;所有文件(*.*)";  //文件过滤器
  //  QString filename =
  //      QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter);
  //  if (filename.isEmpty()) {
  //    return;
  //  }
  //  QFile file(filename);
  //  if (!file.open(QIODevice::WriteOnly)) {
  //    return;
  //  }

  //  /*保存文件*/
  //  QTextStream stream(&file);
  //  //  stream << data;
  //  file.close();
}
void MainWindow::ui_creatStatusBar() {
  slabel = new QLabel(this->ui->statusbar);
  slabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  slabel->setFixedHeight(16);
  slabel->setFixedSize(256 + 64, 16);
  QLabel *label = new QLabel(QString("Rx: "), this->ui->statusbar);
  label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QLabel *label1 = new QLabel(QString("Tx: "), this->ui->statusbar);
  label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  lbTxBytes = new QLabel(this->ui->statusbar);
  lbTxBytes->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  lbTxBytes->setFont(QFont(this->font()));
  lbTxBytes->setFixedSize(128, 16);

  lbRxBytes = new QLabel(this->ui->statusbar);
  lbRxBytes->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  lbRxBytes->setFont(QFont(this->font()));
  lbRxBytes->setFixedSize(128, 16);
  this->ui->statusbar->addPermanentWidget(slabel, 1);
  this->ui->statusbar->addPermanentWidget(label, 1);
  this->ui->statusbar->addPermanentWidget(lbRxBytes, 1);
  this->ui->statusbar->addPermanentWidget(label1, 1);
  this->ui->statusbar->addPermanentWidget(lbTxBytes, 1);
  //  this->ui->statusbar->addPermanentWidget(lbLogPath, 1);
}

void MainWindow::ui_creatToolBar() {
  //  this->ui->toolBar->addAction(this->ui->actionStart);
  //  this->ui->toolBar->addAction(this->ui->actionPause);
  //  this->ui->toolBar->addAction(this->ui->actionStop);
  //  this->ui->toolBar->addAction(this->ui->actionClear);
  //  this->ui->toolBar->addSeparator();
  //  this->ui->toolBar->addAction(this->ui->actionSetting);
}
void MainWindow::ui_showMessage(const QString &message, int time,
                                QColor color) {
  //    this->ui->statusbar->showMessage(message, time);
  //    if(color.isValid())
  //    ui->statusbar->setStyleSheet(QString::fromUtf8("QStatusBar{color:%1;}\n").arg(color.name(QColor::HexArgb)));
  ui_statusbar_showMessage(message, color);
}

int MainWindow::ui_serial_getPortNumber() { return this->ui->cbbPort->count(); }

const QString MainWindow::ui_serial_getPortName() {
  if (ui_serial_getPortNumber() == 0) return QString::fromUtf8("");
  const QString pn = this->ui->cbbPort->currentText();
  int pos = pn.indexOf(']');
  return pn.mid(1, pos - 1);
}

qint64 MainWindow::ui_serial_getBaud() {
  return this->ui->cbbBaud->currentText().toLongLong();
}

QSerialPort::DataBits MainWindow::ui_serial_getDataBit() {
  return QSerialPort::DataBits(this->ui->cbbDataBit->currentText().toInt());
}

QSerialPort::Parity MainWindow::ui_serial_getParity() {
  int pi = this->ui->cbbParity->currentIndex();
  if (pi > 0) {
    pi += 1;
  }
  return QSerialPort::Parity(pi);
}

QSerialPort::StopBits MainWindow::ui_serial_getStopBit() {
  return QSerialPort::StopBits(this->ui->cbbStop->currentIndex() + 1);
}

QSerialPort::FlowControl MainWindow::ui_serial_getFlow() {
  return QSerialPort::FlowControl(this->ui->cbbFlow->currentIndex());
}

void MainWindow::handle_serial_error(int err) {
  QSerialPort::SerialPortError error = (QSerialPort::SerialPortError)err;
  SerialConfig config = ui_serial_getConfig();
  qDebug() << "Serial Error:" << config.errorStr[int(error)];
  switch (error) {
    case QSerialPort::NoError:
      if (serialStatus == STATUS_OPEN) {
        ui_serial_toggle_pbtSend(true);
        const QString mes = QString::fromUtf8("%1 OPENED [%2][%3][%4][%5][%6]")
                                .arg(config.portName)
                                .arg(config.portBaud)
                                .arg(config.portDataBit)
                                .arg(config.parityStr[config.portParity])
                                .arg(config.stopStr[config.portStopBit])
                                .arg(config.flowStr[config.portFlow]);
        ui_showMessage(mes);
      }
      if (serialStatus == STATUS_CLOSE) {
        if (ui_serial_getPortNumber()) {
          const QString mes =
              QString::fromUtf8("%1 CLOSE").arg(config.portName);
          ui_showMessage(mes, 0, Qt::red);
        } else {
          ui_showMessage("");
        }
      }
      break;
    case QSerialPort::TimeoutError:
      break;
    case QSerialPort::NotOpenError:
      if (serialStatus == STATUS_CLOSE) {
        break;
      }
    default:
      ui_serial_toggle_pbtSend(false);
      const QString mes = QString::fromUtf8("%1 串口发生意外错误 [%2]")
                              .arg(ui_serial_getPortName())
                              .arg(config.errorStr[(int)error]);
      ui_showMessage(mes, 0, Qt::black);

      serialStatus = STATUS_OCCUR_ERROR;
      break;
  }
}

void MainWindow::handle_serial_recieve_data(const QByteArray &data, int len) {
  rxBytes += len;
  ui_statusbar_showRxBytes(rxBytes);

  if (settingConfig.recConfig.bufferMode) {
    if (settingConfig.recConfig.bufferSize >
        (recieveBuffer.size() + data.size())) {
      recieveBuffer.append(data);
      return;
    } else {
      recieveBuffer.append(data);
      // show
      ui_showRecieveData(recieveBuffer, recieveBuffer.size());
      recieveBuffer.clear();
    }
  } else {
    if (!recieveBuffer.isEmpty()) {
      ui_showRecieveData(recieveBuffer, recieveBuffer.size());
      recieveBuffer.clear();
    }
    ui_showRecieveData(data, len);
  }

  if (settingConfig.showConfig.enableAutoNewLine) {
  } else {
  }
}
void MainWindow::handle_serialhelper_readyread() {
  qint64 len = 0, tmp = 0;
  QByteArray data;

  tmp = my_serial->bytesAvailable();
  while (tmp > 0) {
    len += tmp;
    data.append(my_serial->readAll());
    tmp = my_serial->bytesAvailable();
  }
  if (serialStatus == STATUS_OPEN) {
    handle_serial_recieve_data(data, len);
  }
}

void MainWindow::on_pbtSend_clicked() {}

void MainWindow::ui_serial_toggle_pbtSend(bool _isOpen) {
  if (_isOpen) {
    this->ui->pbtSend->setText(QString::fromUtf8("发送"));
  } else {
    this->ui->pbtSend->setText(QString::fromUtf8("打开"));
  }
}

void MainWindow::ui_serial_setPortName(QSerialPortInfo info) {
  const QString pn =
      QString::fromUtf8("[%1]%2").arg(info.portName()).arg(info.description());
  this->ui->cbbPort->setCurrentText(pn);
}

void MainWindow::ui_serial_setBaud(qint64 baud) {
  this->ui->cbbBaud->setCurrentText(QString::fromUtf8("%1").arg(baud));
}

void MainWindow::ui_serial_setDataBit(QSerialPort::DataBits d) {
  this->ui->cbbDataBit->setCurrentIndex(
      d - this->ui->cbbDataBit->itemText(0).toInt());
}

void MainWindow::ui_serial_setParity(QSerialPort::Parity p) {
  int pi = (int)p;
  if (pi > 0) pi -= 1;
  this->ui->cbbParity->setCurrentIndex(pi);
}

void MainWindow::ui_serial_setStopBit(QSerialPort::StopBits s) {
  this->ui->cbbStop->setCurrentIndex(s - 1);
}

void MainWindow::ui_serial_setFlow(QSerialPort::FlowControl f) {
  this->ui->cbbFlow->setCurrentIndex(f);
}
void MainWindow::ui_serial_setConfig(SerialConfig config) {
  QSerialPortInfo info(config.portName);
  ui_serial_setPortName(info);
  ui_serial_setBaud(config.portBaud);
  ui_serial_setDataBit(config.portDataBit);
  ui_serial_setParity(config.portParity);
  ui_serial_setStopBit(config.portStopBit);
  ui_serial_setFlow(config.portFlow);
}

const SerialConfig MainWindow::ui_serial_getConfig() {
  SerialConfig config;
  config.portName = ui_serial_getPortName();
  config.portDataBit = ui_serial_getDataBit();
  config.portFlow = ui_serial_getFlow();
  config.portBaud = ui_serial_getBaud();
  config.portParity = ui_serial_getParity();
  config.portStopBit = ui_serial_getStopBit();
  return config;
}

const QString MainWindow::ui_net_getCurrentInterfaceHardAddr() {
  int ns = this->ui->cbbNetworkInterface->count();
  if (ns == 0) return QString("");
  const QString str = this->ui->cbbNetworkInterface->currentText();
  int pos = str.indexOf('[');
  int pos2 = str.indexOf(']');
  return str.mid(pos + 1, pos2 - pos - 1);
}
void MainWindow::ui_net_setCurrentInterface(QNetworkInterface interface) {
  const QString str = ui_net_makeInterfaceStr(interface);
  int find = this->ui->cbbNetworkInterface->findText(str);

  if (find != -1) {
    this->ui->cbbNetworkInterface->setCurrentIndex(find);
  } else {
    this->ui->cbbNetworkInterface->addItem(str);
    this->ui->cbbNetworkInterface->setCurrentIndex(
        this->ui->cbbNetworkInterface->count() - 1);
  }
}
void MainWindow::ui_net_addInterface(QNetworkInterface interface) {
  this->ui->cbbNetworkInterface->addItem(ui_net_makeInterfaceStr(interface));
}

void MainWindow::ui_statusbar_showRxBytes(ulong bytes) {
  this->lbRxBytes->setText(QString::fromUtf8("%1 Bytes").arg(bytes));
}

void MainWindow::ui_statusbar_showTxBytes(ulong bytes) {
  this->lbTxBytes->setText(QString::fromUtf8("%1 Bytes").arg(bytes));
}
void MainWindow::ui_statusbar_showMessage(const QString &str, QColor color) {
  this->slabel->setText(str);
  this->slabel->setStyleSheet(
      QString::fromUtf8("QLabel{color:%1;font:%2pt \"%3\";}")
          .arg(color.name(QColor::HexRgb))
          .arg(this->font().pointSize())
          .arg(this->font().family()));
}

void MainWindow::ui_showRecieve(const QString &s, bool t) {
  QString hStr;
  QString dStr;
  QString str = s;

  int pos = this->ui->pteShowRecieve->textCursor().position();
  if (pos > (settingConfig.showConfig.bufferSize << 10 << 10)) {
    ui_clearRecieve();
  }

  if (t) {
    dStr = QDateTime::currentDateTime().toString("[yyyy-MM-dd:hh:mm:ss.zzz] ");
    hStr.append(
        QString::fromUtf8("<i style=\"color:#999999;\">%1</i>").arg(dStr));
  }

  if (settingConfig.logConfig.enableSaveLog) {
    //    ui_log_logSaveToFile(QString(dStr + str));
  }

  if (settingConfig.showConfig.enableShowColor) {
    hStr.append(QString::fromUtf8("<big style=\"color:%1;\">%2</big>")
                    .arg(settingConfig.showConfig.recColor.name(QColor::HexRgb))
                    .arg(str));
  } else {
    hStr.append(
        QString::fromUtf8("<big style=\"color:black;\">%1</big>").arg(str));
  }

  hStr.replace(QLatin1String(settingConfig.lineEnd[settingConfig.lineMode]),
               QLatin1String("<br />"));

  this->ui->pteShowRecieve->appendHtml(hStr);
  this->ui->pteShowRecieve->moveCursor(QTextCursor::End);
}
void MainWindow::ui_clearRecieve() {
  this->ui->pteShowRecieve->clear();
  this->ui->pteShowRecieve->moveCursor(QTextCursor::Start);

  if (!recieveBuffer.isEmpty()) recieveBuffer.clear();

  txBytes = 0;
  rxBytes = 0;

  ui_statusbar_showTxBytes(txBytes);
  ui_statusbar_showRxBytes(rxBytes);
}

void MainWindow::ui_showRecieveData(const QByteArray &data, int _len) {
  int len = _len;

  if (settingConfig.recConfig.showMode == ASCII_MODE) {
    ui_showRecieve(QString::fromUtf8(data, len),
                   settingConfig.showConfig.enableShowTime);
    return;
  }
  if (settingConfig.recConfig.showMode == HEX_MODE) {
    const QByteArray hex = data.toHex().toUpper();
    len = hex.size();
    QByteArray bytes(len + len / 2 + 2, '\0');
    int i = 0, j = 0;
    for (i = 0; i < len;) {
      bytes[j] = hex[i];
      bytes[j + 1] = hex[i + 1];
      bytes[j + 2] = ' ';
      j += 3;
      i += 2;
    }
    ui_showRecieve(QString::fromUtf8(bytes, j),
                   settingConfig.showConfig.enableShowTime);
  }
}

#if 0

void MainWindow::on_sendButton_clicked() {
#if 0
    QString strdata;
    if (ui->swButton->text() == "打开灯") {
        ui->swButton->setText(tr("关闭灯"));
        strdata = QString("ON\n");
        if (serialPort->write("ON\n") < 0) {
            serialPort->close();
            ui->openButton->setEnabled(true);
        }
        ui->readtextEdit->setTextColor(QColor("red"));
        ui->readtextEdit->append(
            QString("[%1]TX -> ")
                .arg(QTime::currentTime().toString("HH:mm:ss:zzz")));
        ui->readtextEdit->setTextColor(QColor("black"));
        ui->readtextEdit->insertPlainText(strdata);
        qDebug("ON\n");
    } else if (ui->swButton->text() == "关闭灯") {
        ui->swButton->setText(tr("打开灯"));
        strdata = QString("OFF\n");

        if (serialPort->write("OFF\n") < 0) {
            serialPort->close();
            ui->openButton->setEnabled(true);
        }
        ui->readtextEdit->setTextColor(QColor("red"));
        ui->readtextEdit->append(
            QString("[%1]TX -> ")
                .arg(QTime::currentTime().toString("HH:mm:ss:zzz")));
        ui->readtextEdit->setTextColor(QColor("black"));
        ui->readtextEdit->insertPlainText(strdata);
        qDebug("OFF\n");
    }
#endif
  serialPortWrite();
}
#endif
