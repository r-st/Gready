#ifndef READER_H
#define READER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QDebug>
#include <QCoreApplication>
#include <QSettings>

class Reader : public QObject {
  Q_OBJECT

private:
  QString m_sid;
  QNetworkAccessManager m_manager;
  QSettings m_settings;
  
  /**
   * Get session ID from Google
   */
  void getID();

public:
  Reader();
  QString printID();
  void quit() { QCoreApplication::instance()->quit(); }

public slots:
  /**
   * Process SID retrieved from getID
   * @param reply response from server
   */
  void authenticated(QNetworkReply* reply);
  
};

#endif // READER_H
