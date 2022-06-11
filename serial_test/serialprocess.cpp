#include "serialprocess.h"

#include <QDebug>
SerialProcess::SerialProcess(QObject *parent) {
  m_port = new QSerialPort(parent);
  s_err_connect =
      QObject::connect(m_port, &QSerialPort::errorOccurred, this,
                       &SerialProcess::handle_QSerialPort_errorOccurred);
}

SerialProcess::SerialProcess(const QSerialPortInfo &info, QObject *parent) {
  m_port = new QSerialPort(info, parent);
  s_err_connect =
      QObject::connect(m_port, &QSerialPort::errorOccurred, this,
                       &SerialProcess::handle_QSerialPort_errorOccurred);
}

SerialProcess::SerialProcess(const QString &name, QObject *parent) {
  m_port = new QSerialPort(name, parent);
  s_err_connect =
      QObject::connect(m_port, &QSerialPort::errorOccurred, this,
                       &SerialProcess::handle_QSerialPort_errorOccurred);
}

SerialProcess::~SerialProcess() {
  if (m_port) {
    if (m_port->isOpen())
      m_port->close();
    QObject::disconnect(s_err_connect);
    m_port->deleteLater();
  }
  if (m_timer) {
    if (m_timer->isActive())
      m_timer->stop();
  }
}

QSerialPort *SerialProcess::port() { return this->m_port; }

void SerialProcess::setPort(const QSerialPortInfo &info) {
  if (m_port) {
    if (m_port->isOpen()) {
      m_port->close();
    }

    QObject::disconnect(s_err_connect);

    m_port->deleteLater();
  }
  m_port = new QSerialPort(info);
  s_err_connect =
      QObject::connect(m_port, &QSerialPort::errorOccurred, this,
                       &SerialProcess::handle_QSerialPort_errorOccurred);
}

bool SerialProcess::open(QIODevice::OpenMode mode) {
  if (!m_port)
    return false;
  //    if(m_port->isOpen()) return true;

  return m_port->open(mode);
}

void SerialProcess::close() {
  if (!m_port)
    return;
  if (m_port->isOpen())
    m_port->close();
  //    QObject::disconnect(s_err_connect);
}

bool SerialProcess::isOpen() const {
  if (!m_port)
    return false;
  return m_port->isOpen();
}

bool SerialProcess::isSequential() const {
  if (!m_port)
    return false;
  return m_port->isSequential();
}

qint64 SerialProcess::bytesAvailable() const {
  if (!m_port)
    return -1;
  return m_port->bytesAvailable();
}

qint64 SerialProcess::bytesToWrite() const {
  if (!m_port)
    return -1;
  return m_port->bytesToWrite();
}

bool SerialProcess::canReadLine() const {
  if (!m_port)
    return false;
  return m_port->canReadLine();
}

void SerialProcess::setPortName(const QString &name) {
  if (!m_port)
    return;
  m_port->setPortName(name);
}

QString SerialProcess::portName() const {
  if (!m_port)
    return QString();
  return m_port->portName();
}

bool SerialProcess::setBaudRate(qint32 baudRate,
                                QSerialPort::Directions directions) {
  if (!m_port)
    return false;
  return m_port->setBaudRate(baudRate, directions);
}

qint32 SerialProcess::baudRate(QSerialPort::Directions directions) const {
  if (!m_port)
    return -1;
  return m_port->baudRate(directions);
}

bool SerialProcess::setDataBits(QSerialPort::DataBits dataBits) {
  if (!m_port)
    return false;
  return m_port->setDataBits(dataBits);
}

// QSerialPort::DataBits SerialProcess::dataBits() const {
//   if (!m_port)
//     return QSerialPort::UnknownDataBits;
//   return m_port->dataBits();
// }

bool SerialProcess::setParity(QSerialPort::Parity parity) {
  if (!m_port)
    return false;
  return m_port->setParity(parity);
}

// QSerialPort::Parity SerialProcess::parity() const {
//   if (!m_port)
//     return false;
//   //  QSerialPort::UnknownParity;
//   return m_port->parity();
// }

bool SerialProcess::setStopBits(QSerialPort::StopBits stopBits) {
  if (!m_port)
    return false;
  return m_port->setStopBits(stopBits);
}

// QSerialPort::StopBits SerialProcess::stopBits() const {
//   if (!m_port)
//     return false;
//   //  QSerialPort::UnknownStopBits;
//   return m_port->stopBits();
// }

bool SerialProcess::setFlowControl(QSerialPort::FlowControl flowControl) {
  if (!m_port)
    return false;
  return m_port->setFlowControl(flowControl);
}

// QSerialPort::FlowControl SerialProcess::flowControl() const {
//   if (!m_port)
//     return false;
//   //  QSerialPort::UnknownFlowControl;
//   return m_port->flowControl();
// }

bool SerialProcess::setDataTerminalReady(bool set) {
  if (!m_port)
    return false;
  return m_port->setDataTerminalReady(set);
}

bool SerialProcess::isDataTerminalReady() {
  if (!m_port)
    return false;
  return m_port->isDataTerminalReady();
}

bool SerialProcess::setRequestToSend(bool set) {
  if (!m_port)
    return false;
  return m_port->setRequestToSend(set);
}

bool SerialProcess::isRequestToSend() {
  if (!m_port)
    return false;
  return m_port->isRequestToSend();
}

QSerialPort::PinoutSignals SerialProcess::pinoutSignals() {
  if (!m_port)
    return QSerialPort::NoSignal;
  return m_port->pinoutSignals();
}

bool SerialProcess::flush() {
  if (!m_port)
    return false;
  return m_port->flush();
}

bool SerialProcess::clear(QSerialPort::Directions directions) {
  if (!m_port)
    return false;
  return m_port->clear(directions);
}

qint64 SerialProcess::readBufferSize() const {
  if (!m_port)
    return -1;
  return m_port->readBufferSize();
}

void SerialProcess::setReadBufferSize(qint64 size) {
  if (!m_port)
    return;
  m_port->setReadBufferSize(size);
}

qint64 SerialProcess::write(const char *data, qint64 len) {
  if (!m_port)
    return -1;
  return m_port->write(data, len);
}

qint64 SerialProcess::write(const char *data) {
  if (!m_port)
    return -1;
  return m_port->write(data);
}

qint64 SerialProcess::read(char *data, qint64 maxlen) {
  if (!m_port)
    return -1;
  return m_port->read(data, maxlen);
}

QByteArray SerialProcess::read(qint64 maxlen) {
  if (!m_port)
    return QByteArray();
  return m_port->read(maxlen);
}

QByteArray SerialProcess::readAll() {
  if (!m_port)
    return QByteArray();
  return m_port->readAll();
}

qint64 SerialProcess::readLine(char *data, qint64 maxlen) {
  if (!m_port)
    return -1;
  return m_port->readLine(data, maxlen);
}

QByteArray SerialProcess::readLine(qint64 maxlen) {
  if (!m_port)
    return QByteArray();
  return m_port->readLine(maxlen);
}

void SerialProcess::setAutoWritePriod(int msec) {
  m_priod = msec;
  if (!m_timer)
    return;
  bool ts = m_timer->isActive();

  if (ts)
    m_timer->stop();
  m_timer->setInterval(m_priod);
  if (ts)
    m_timer->start();
}

int SerialProcess::autoWritePriod() { return m_priod; }

void SerialProcess::startAutoWrite(int msec) {
  if (!m_port)
    return;
  if (!m_port->isOpen())
    return;

  if (!m_timer) {
    m_timer = new QTimer();
    t_timeout_connect = QObject::connect(m_timer, &QTimer::timeout, this,
                                         &SerialProcess::handle_QTimer_timeout);
  }
  m_priod = msec;
  if (m_timer->isActive())
    m_timer->stop();
  m_timer->setInterval(m_priod);
  if (!m_timer->isActive())
    m_timer->start();
}

void SerialProcess::startAutoWrite(const QByteArray &data, int msec) {
  m_auto_data = data;
  startAutoWrite(msec);
}

void SerialProcess::setAutoWrite(const QByteArray &data) { m_auto_data = data; }

void SerialProcess::stopAutoWrite() {
  if (!m_timer)
    return;
  if (m_timer->isActive())
    m_timer->stop();
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
