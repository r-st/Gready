#ifndef READER_H
#define READER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QCoreApplication>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QDateTime>

class Reader : public QObject {
  Q_OBJECT

private:
  QString m_sid;
  QNetworkAccessManager m_manager;
  QSettings m_settings;
  QNetworkCookieJar* m_cookieJar;
  
  /**
   * Get session ID from Google
   */
  void getID();
  
  /**
   * Set cookie with SID
   */
  void setCookie();

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
