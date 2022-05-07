#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTime>

#include "ui_mainwindow.h"
static int dataTotalRxCnt = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowTitle("serial control");
  serialPort = new QSerialPort(this);
  scanSerialPort();
  /* 接收数据信号槽 */
  connect(serialPort, &QSerialPort::readyRead, this,
          &MainWindow::SerialPortReadyRead_slot);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::scanSerialPort() {
  QStringList serialNamePort;

  /* 搜索所有可用串口 */
  foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()) {
    serialNamePort << inf0.portName();
  }
  ui->serialBox->addItems(serialNamePort);
}
void MainWindow::on_openButton_clicked() {
  if (ui->openButton->text() == "打开串口") {
    /* 串口设置 */
    serialPort->setPortName(ui->serialBox->currentText());
    serialPort->setBaudRate(ui->baudrateBox->currentText().toInt());
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    /* 打开串口提示框 */
    if (true == serialPort->open(QIODevice::ReadWrite)) {
      QMessageBox::information(this, "提示", "串口打开成功");
      //      ui->openButton->setEnabled(false);
      ui->serialBox->setEnabled(false);
      ui->baudrateBox->setEnabled(false);
      ui->openButton->setText(tr("关闭串口"));

    } else {
      QMessageBox::critical(this, "提示", "串口打开失败");
    }
  } else if (ui->openButton->text() == "关闭串口") {
    ui->openButton->setText(tr("打开串口"));
    serialPort->close();
    ui->serialBox->setEnabled(true);
    ui->baudrateBox->setEnabled(true);
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

  /*数据是否为空*/
  if (!bytedata.isEmpty()) {
    if (ui->HexDispcheckBox->isChecked()) {
      /*hex显示*/
      framedata = bytedata.toHex(' ').trimmed().toUpper();
      ui->readtextEdit->setTextColor(QColor(Qt::green));
    } else {
      /*ascii显示*/
      framedata = QString(bytedata);
      ui->readtextEdit->setTextColor(QColor(Qt::magenta));
    }
    if (ui->TimeDispcheckBox->isChecked()) {
      /*是否显示时间戳*/

      framedata = QString("[%1]:RX -> %2")
                      .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
                      .arg(framedata);
      ui->readtextEdit->append(framedata);
    } else {
      ui->readtextEdit->insertPlainText(framedata);
    }
    /*更新接收计数*/
    dataTotalRxCnt += bytedata.length();
    ui->rxCnt_label->setText(QString::number(dataTotalRxCnt));
  }
}

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

void MainWindow::on_clearRxButton_clicked() {
  ui->readtextEdit->clear();
  ui->rxCnt_label->setText(QString::number(0));
  dataTotalRxCnt = 0;
}
void MainWindow::serialPortWrite() {
  QByteArray SendTextEditBa;
  QString SendTextEditStr;
  //获取发送框字符

  SendTextEditStr = ui->sendtextEdit->document()->toPlainText();
  //判断是否非空
  if (SendTextEditStr.isEmpty()) {
    return;
  }
  if (ui->HexSendcheckBox->isChecked()) {
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
  QString data = ui->readtextEdit->toPlainText();

  if (data.isEmpty()) {
    QMessageBox::information(this, "提示", "数据内容空");
    return;
  }

  QString curPath = QDir::currentPath();              //获取系统当前目录
  QString dlgTitle = "保存文件";                      //对话框标题
  QString filter = "文本文件(*.txt);;所有文件(*.*)";  //文件过滤器
  QString filename =
      QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter);
  if (filename.isEmpty()) {
    return;
  }
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return;
  }

  /*保存文件*/
  QTextStream stream(&file);
  stream << data;
  file.close();
}
