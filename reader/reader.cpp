#include "./reader.h"

Reader::Reader()
{
  //connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(authenticated(QNetworkReply*)));
  
  getID();

}


void Reader::getID() {
  QUrl params;
  QByteArray data;

  // Prepare authentication data
  params.addQueryItem("service", "reader");
  params.addEncodedQueryItem("Email", m_settings.value("username").toByteArray());
  params.addEncodedQueryItem("Passwd", m_settings.value("password").toByteArray());
  params.addEncodedQueryItem("source", "gready");
  params.addEncodedQueryItem("continue", "http://www.google.com/");

  data = params.encodedQuery();

  QNetworkRequest req(QUrl("https://www.google.com/accounts/ClientLogin"));
  
  m_reply =  m_manager.post(req, data);
  connect(m_reply, SIGNAL(finished()), SLOT(authenticated()));
}

void Reader::authenticated() {
  
  if(m_reply->error()) { qDebug() << m_reply->errorString(); }
  else {
    // remove key
    QString sid = m_reply->readLine().remove(0,4);
    // delete newline
    sid.chop(1);
    m_sid = sid;
    qDebug()<<m_sid;
  }
  
  m_reply->deleteLater();
}

QString Reader::printID() {
  
  return m_sid;
}

void Reader::setCookie() {
  QNetworkCookie cookie;
  
  cookie.setName(QByteArray(m_sid.toAscii()));
  cookie.setDomain(QByteArray(".google.com"));
  cookie.setPath(QByteArray("/"));
  cookie.setExpirationDate(QDateTime());
  
  QList<QNetworkCookie> list;
  list.append(cookie);
  m_cookieJar->setCookiesFromUrl(list, QUrl("http://www.google.com/reader/"));
  
  m_manager.setCookieJar(m_cookieJar);
}


#include "reader.moc"

