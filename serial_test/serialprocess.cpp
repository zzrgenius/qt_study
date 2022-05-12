#include "serialprocess.h"

#include <QDebug>
SerialProcess::SerialProcess(QObject *parent) {
  m_port = new QSerialPort(parent);
  s_err_connect =
      QObject::connect(m_port, &QSerialPort::errorOccurred, this,
                       &SerialProcess::handle_QSerialPort_errorOccurred);
}
SerialProcess::~SerialProcess() {
  if (m_port) {
    if (m_port->isOpen()) m_port->close();
    QObject::disconnect(s_err_connect);
    m_port->deleteLater();
  }
  if (m_timer) {
    if (m_timer->isActive()) m_timer->stop();
  }
}

void SerialProcess::startAutoWrite(int msec) {
  if (!m_port) return;
  if (!m_port->isOpen()) return;

  if (!m_timer) {
    m_timer = new QTimer();
    t_timeout_connect = QObject::connect(m_timer, &QTimer::timeout, this,
                                         &SerialProcess::handle_QTimer_timeout);
  }
  m_priod = msec;
  if (m_timer->isActive()) m_timer->stop();
  m_timer->setInterval(m_priod);
  if (!m_timer->isActive()) m_timer->start();
}

void SerialProcess::startAutoWrite(const QByteArray &data, int msec) {
  m_auto_data = data;
  startAutoWrite(msec);
}
void SerialProcess::setAutoWrite(const QByteArray &data) { m_auto_data = data; }
void SerialProcess::stopAutoWrite() {
  if (!m_timer) return;
  if (m_timer->isActive()) m_timer->stop();
  QObject::disconnect(t_timeout_connect);
  m_timer->deleteLater();
  m_timer = nullptr;
}

void SerialProcess::handle_QSerialPort_errorOccurred(
    QSerialPort::SerialPortError error) {
  //    qDebug() << error;
  int err = (int)error;
  emit SerialProcess::errorOccurred(err);
}

void SerialProcess::handle_QTimer_timeout() {
  SerialProcess::write(m_auto_data);
}
