#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
        QStringList serialNamePort;
    ui->setupUi(this);
    this->setWindowTitle("serial control");
    serialPort = new QSerialPort(this);
    /* 搜索所有可用串口 */
        foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()) {
            serialNamePort<<inf0.portName();
        }
        ui->serialBox->addItems(serialNamePort);
        /* 接收数据信号槽 */
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::SerialPortReadyRead_slot);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openButton_clicked()
{
    /* 串口设置 */
        serialPort->setPortName(ui->serialBox->currentText());
        serialPort->setBaudRate(ui->baudrateBox->currentText().toInt());
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setParity(QSerialPort::NoParity);

        /* 打开串口提示框 */
        if (true == serialPort->open(QIODevice::ReadWrite))
        {
            QMessageBox::information(this, "提示", "串口打开成功");
            ui->openButton->setEnabled(false);

        }
        else
        {
            QMessageBox::critical(this, "提示", "串口打开失败");
        }
}


void MainWindow::on_closeButton_clicked()
{
        serialPort->close();
        ui->openButton->setEnabled(true);

}


void MainWindow::on_swButton_clicked()
{
//    static int flag = 0;
     if(ui->swButton->text() == "打开灯")
     {
         ui->swButton->setText(tr("关闭灯"));
         if(serialPort->write("ON\n") < 0)
        {
            serialPort->close();
            ui->openButton->setEnabled(true);

        }
        qDebug("ON\n");
     }else if(ui->swButton->text() == "关闭灯")
     {
         ui->swButton->setText(tr("打开灯"));

         if(serialPort->write("OFF\n") < 0)
         {
             serialPort->close();
             ui->openButton->setEnabled(true);


         }
         qDebug("OFF\n");
     }


}
/*
    函   数：SerialPortReadyRead_slot
    描   述：readyRead()信号对应的数据接收槽函数
    输   入：无
    输   出：无
*/
void MainWindow::SerialPortReadyRead_slot()
{
//    QTime *time = new QTime();
    static int dataTotalRxCnt = 0;
        QString framedata;
        /*读取串口收到的数据*/
        QByteArray bytedata = serialPort->readAll();

        /*数据是否为空*/
        if (!bytedata.isEmpty())
        {

            {
                /*ascii显示*/
                framedata = QString(bytedata);
                ui->readtextEdit->setTextColor(QColor(Qt::magenta));
            }

            /*是否显示时间戳*/

                framedata = QString("[%1]:RX -> %2").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(framedata);
                ui->readtextEdit->append(framedata);



            /*更新接收计数*/
            dataTotalRxCnt += bytedata.length();
//            ui->RxCnt_label->setText(QString::number(dataTotalRxCnt));
        }

}
