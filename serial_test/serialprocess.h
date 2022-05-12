#ifndef SERIALPROCESS_H
#define SERIALPROCESS_H
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QList>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QTimer>
class SerialProcess : public QSerialPort {
  Q_OBJECT

 public:
  explicit SerialProcess(QObject *parent = nullptr);
  explicit SerialProcess(const QSerialPortInfo &info,
                         QObject *parent = nullptr);
  explicit SerialProcess(const QString &name, QObject *parent = nullptr);
  ~SerialProcess();
  //  //-------------------->
  //  qint64 write(const char *data, qint64 len);
  //  qint64 write(const char *data);
  //  //  inline qint64 write(const QByteArray &data) {
  //  //    return write(data.constData(), data.size());
  //  //  }

  //  qint64 read(char *data, qint64 maxlen);
  //  QByteArray read(qint64 maxlen);
  //  QByteArray readAll();
  //  qint64 readLine(char *data, qint64 maxlen);
  //  QByteArray readLine(qint64 maxlen = 0);

  // <------------------------------>
  void setAutoWritePriod(int msec = 0);
  int autoWritePriod(void);
  void setAutoWrite(const QByteArray &data);
  void startAutoWrite(int msec = 0);
  void startAutoWrite(const QByteArray &data, int msec = 1000);
  void stopAutoWrite(void);
  template <typename... Args>
  QMetaObject::Connection callOnReadyRead(Args &&...args) {
    if (!m_port) return QMetaObject::Connection();
    //        s_connect = QObject::connect(m_port, &QSerialPort::readyRead,
    //        std::forward<Args>(args)...); return s_connect;
    return QObject::connect(m_port, &QSerialPort::readyRead,
                            std::forward<Args>(args)...);
  }

 signals:
  void errorOccurred(int err);

 private:
  QSerialPort *m_port = nullptr;

  //    QMetaObject::Connection s_connect;
  QMetaObject::Connection s_err_connect;
  QTimer *m_timer = nullptr;
  int m_priod = 0;
  QByteArray m_auto_data;
  QMetaObject::Connection t_timeout_connect;

  void handle_QSerialPort_errorOccurred(QSerialPort::SerialPortError error);

  void handle_QTimer_timeout(void);
};
#endif  // SERIALPROCESS_H
