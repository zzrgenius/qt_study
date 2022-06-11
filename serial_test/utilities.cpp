#include "mainwindow.h"
#include <QTextCodec>

QString HexToAscII(QString text) {
  QTextCodec *tc = QTextCodec::codecForName("System");
  QString tmp;
  QByteArray ascii_data;
  //  QStringList temp_list = text.split(" ", QString::SkipEmptyParts);
  QStringList temp_list = text.split(" ");

  foreach (QString str, temp_list) { ascii_data.append(str.toUtf8()); }
  tmp = tc->toUnicode(QByteArray::fromHex(ascii_data));
  return tmp;
}
