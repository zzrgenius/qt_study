#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
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
 private slots:
  void on_openButton_clicked();
  void SerialPortReadyRead_slot();
  void on_sendButton_clicked();

  void on_clearRxButton_clicked();
  void serialPortWrite();

  void on_SaveRxBufButton_clicked();

 private:
  Ui::MainWindow *ui;
  QSerialPort *serialPort;
  //扫描串口
  void scanSerialPort();
};
#endif  // MAINWINDOW_H
