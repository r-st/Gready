/*
    Copyright (c) 2011 Robert Štětka <robert.stetka@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include "./reader.h"

Reader::Reader():m_apiUrl("http://www.google.com/reader/api/0/"), m_atomUrl("http://www.google.com/reader/atom/") {
    m_signalMapper = new QSignalMapper(this);
    getID();

    m_tokenTimer = new QTimer(this);
}


void Reader::getID() {
  if(!m_settings.value("username").toString().isEmpty() && !m_settings.value("password").toString().isEmpty()) {
    QUrl params("https://www.google.com/accounts/ClientLogin");

    // Prepare authentication data
    params.addQueryItem("service", "reader");
    params.addEncodedQueryItem("Email", m_settings.value("username").toByteArray());
    params.addEncodedQueryItem("Passwd", m_settings.value("password").toByteArray());
    params.addEncodedQueryItem("source", QByteArray(m_settings.applicationName().toAscii()));
    params.addEncodedQueryItem("continue", "http://www.google.com/");


    QNetworkRequest req(params);

    QNetworkReply* reply =  m_manager.get(req);
    connect(reply, SIGNAL(finished()), SLOT(authenticated()));
  }
}

void Reader::authenticated() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Auth: " << reply->errorString();
    } else {
        // skip two lines
        reply->readLine();
        reply->readLine();

        // remove key and get value
        QString autVal = reply->readLine().remove(0,4);
        // delete newline
        autVal.chop(1);
        m_authKey = autVal;
    }

    reply->deleteLater();
    emit authenticationDone();
    getToken();
}

QNetworkRequest Reader::setAuthHeader(QNetworkRequest req) {
    req.setRawHeader(QByteArray("Authorization"), QByteArray("GoogleLogin auth").append(m_authKey));

    return req;
}

void Reader::getTags() {
    QString url = m_apiUrl;
    url.append("tag/list");
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

    QUrl request(url);
    request.addQueryItem(QString("output"),QString("json"));
    request.addQueryItem(QString("ck"), QString::number(timestamp));
    request.addQueryItem(QString("client"), m_settings.applicationName());


    QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
    connect(reply, SIGNAL(finished()), SLOT(taglistFinished()));
}

void Reader::taglistFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {
        QJson::Parser json_parser;
        bool ok;
        QVariantMap result;
        
        result = json_parser.parse(reply->readAll(), &ok).toMap();
        if (!ok) {
            // TODO: Process error
            qFatal("Tags: parsing error");
            exit(1);
        }
        
        foreach(QVariant item, result["tags"].toList()) {
          QString elementText =  item.toMap()["id"].toString();
          QString name;
        Tag::type type;

        // state element
        if (elementText.contains("state/com.")) {
            if (elementText.endsWith("starred")) {
                type = Tag::Starred;
                name = "Starred";
            } else if (elementText.endsWith("broadcast")) {
                type = Tag::Shared;
                name = "Shared";
            } else if (elementText.endsWith("blogger-following")) {
                type = Tag::Following;
                name = "Following";
            } else {
                type = Tag::StateUnknown;
                QString indexStr = "/state/";
                int index = elementText.indexOf(indexStr);
                name = elementText.mid(index + indexStr.length());
            }
            // label element
        } else {
            type = Tag::Label;
            QString indexStr = "/label/";
            int index = elementText.indexOf(indexStr);
            name = elementText.mid(index + indexStr.length());
        }
        // Create new tag
        Tag* newTag = new Tag(name,elementText,type);

        // Save it
        m_tagList.insert(name, newTag);
        m_tagListByID.insert(elementText, newTag);
      }
      emit tagsFetchingDone();
    }

    reply->deleteLater();
}

void Reader::getAllFeeds()
{
    QString url = m_apiUrl;
    url.append("subscription/list");
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

    QUrl request(url);
    request.addQueryItem(QString("output"),QString("json"));
    request.addQueryItem(QString("ck"), QString::number(timestamp));
    request.addQueryItem(QString("client"), m_settings.applicationName());


    QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
    connect(reply, SIGNAL(finished()), SLOT(feedsFetched()));
}

void Reader::feedsFetched()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {
      QJson::Parser json_parser;
      QVariantMap result;
      bool ok;
      
      result = json_parser.parse(reply->readAll(), &ok).toMap();
      if (!ok) {
          // TODO: Process error
          qFatal("Feeds: parsing error");
          exit(1);
      }
      foreach(QVariant item, result["subscriptions"].toList()) {
        QString id = item.toMap()["id"].toString();
        QString title = item.toMap()["title"].toString();
        bool category = false;
        Feed* newFeed = new Feed(title, id, this);
        foreach(QVariant cat, item.toMap()["categories"].toList()) {
          newFeed->setHasCategory(true);
          QString category = cat.toMap()["label"].toString();
          m_tagList[category]->addFeed(newFeed);
        }
        
        m_feedList.insert(title, newFeed);
        m_feedListByID.insert(id, newFeed);
      }
      emit feedsFetchingDone();
    }
    reply->deleteLater();
}


void Reader::getArticlesFromFeed(QString feedName, QString continuationId)
{
    QString url = m_apiUrl;
    url.append("stream/contents/");
    if (!m_feedList.contains(feedName)) {
        // TODO: error handling
        qDebug() << "Error: bad feed name";
    } else {
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

        QUrl request(url);
        // set the path propperly
        // it prevents breaking the path when original url contains question mark
        request.setPath(request.path() + m_feedList[feedName]->getID());
        request.addQueryItem(QString("ck"), QString::number(timestamp));
        request.addQueryItem(QString("client"), m_settings.applicationName());
        
        if(!continuationId.isEmpty()) {
          request.addQueryItem(QString("c"), continuationId);
        }

        QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
        connect(reply, SIGNAL(finished()), m_signalMapper, SLOT(map()));

        m_signalMapper->setMapping(reply, feedName);
        connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(articlesFromFeedFinished(QString)));
    }
}
void Reader::articlesFromFeedFinished(QString feedName)
{

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(m_signalMapper->mapping(feedName));
    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {
        Feed* feed = m_feedList[feedName];
        QJson::Parser json_parser;
        QVariantMap result;
        bool ok;
      
        result = json_parser.parse(reply->readAll(), &ok).toMap();
        if (!ok) {
          // TODO: Process error
          qFatal("Feeds: parsing error");
          exit(1);
        }
        
        feed->setContinuation(result["continuation"].toString());
        foreach(QVariant item, result["items"].toList()) {
          QDateTime published;
          QUrl articleUrl;
          QString author;
          QMimeData articleContent;
          bool isRead = false;
          bool isStarred = false;
          bool isShared = false;
          QString articleId;
          QString title;
        
          articleUrl = item.toMap()["alternate"].toList().first().toMap()["href"].toString();
          author = item.toMap()["author"].toString();
          articleId = item.toMap()["id"].toString();
          title = item.toMap()["title"].toString();
          published = QDateTime::fromTime_t(item.toMap()["published"].toUInt());
          articleContent.setHtml(item.toMap()["summary"].toMap()["content"].toString());
          
          foreach(QVariant cat,  item.toMap()["categories"].toList()) {
            if(cat.toString().endsWith("read")) { isRead = true; }
            else if(cat.toString().endsWith("starred")) { isStarred = true; }
            else if(cat.toString().endsWith("broadcast")) { isShared = true; }
          }
          Article* newArticle = new Article(title, author, articleId, articleUrl, articleContent, published, isRead, isStarred, isShared);
          m_feedList[feedName]->addArticle(articleId, newArticle);
        }
        
      emit articlesFetchingDone(m_feedList[feedName]);
      
    }
    reply->deleteLater();
}

void Reader::getUnreadCount()
{
    QString url = m_apiUrl;
    url.append("unread-count");
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

    QUrl request(url);
    request.addQueryItem(QString("output"),QString("json"));
    request.addQueryItem(QString("ck"), QString::number(timestamp));
    request.addQueryItem(QString("client"), m_settings.applicationName());


    QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
    connect(reply, SIGNAL(finished()), SLOT(unreadFinished()));
}

void Reader::unreadFinished()
{

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Unread: " << reply->errorString();
    } else {
      QJson::Parser json_parser;
      QVariantMap result;
      bool ok;
      
      result = json_parser.parse(reply->readAll(), &ok).toMap();
      if (!ok) {
        // TODO: Process error
        qFatal("Feeds: parsing error");
        exit(1);
      }
      
      foreach(QVariant item, result["unreadcounts"].toList()) {
        QString id = item.toMap()["id"].toString();
        uint unread = item.toMap()["count"].toUInt();
        if(id.startsWith("feed")) {
          if(m_feedListByID.contains(id)) {
            m_feedListByID[id]->setUnreadCount(unread);
          }
        } else if(id.startsWith("user")) {
          if(m_tagListByID.contains(id)) {
            m_tagListByID[id]->setUnreadCount(unread);
          }
        }
      }
      emit unreadCountDone();
    }
    
    reply->deleteLater();
}

void Reader::getToken()
{
  QString url = m_apiUrl;
  url.append("token");
  
  QUrl request(url);
  QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
  
  connect(reply, SIGNAL(finished()), SLOT(tokenFinished()));
}

void Reader::tokenFinished()
{ 
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

  if(reply->error()) {
    // TODO
    qDebug() << "Token: " << reply->errorString();
  } else {
    m_token = reply->readAll();
    
    emit tokenDone();
    
    if(!m_tokenTimer->isActive()) {
      // token needs to be updated every 30 minutes
      connect(m_tokenTimer, SIGNAL(timeout()), SLOT(getToken()));
      m_tokenTimer->start(30*60*1000);
    }
  }
}

void Reader::editTag(QString articleId, QString feedName, Reader::editAction action, Reader::tagIdentifier tag, QString tagName)
{
  QString url = m_apiUrl;
  url.append("edit-tag");
  
  QUrl request(url);
  request.addEncodedQueryItem("client", QByteArray(m_settings.applicationName().toAscii()));
  
  QUrl params;
  // add or remove tag
  QString addOrRemove = action == Add ? "a" : "r";
  
  QString label;
  
  switch(tag) {
    case Read:
      label = "user/-/state/com.google/read";
      break;
    case Star:
      label = "user/-/state/com.google/starred";
      break;
    case KeepUnread:
      label = "user/-/state/com.google/tracking-kept-unread";
      break;
    case Like:
      label = "user/-/state/com.google/like";
      break;
    case Share:
      label = "user/-/state/com.google/broadcast";
      break;
    case Label:
      if(!tagName.isEmpty()) {
        label = "user/-/label/" + tagName;
      } else {
        // TODO: error handling
        qDebug() << "Edit label: Empty label name";
      }
      break;
  }
  
  params.addQueryItem(addOrRemove, label);
  
  // add feed
  params.addQueryItem("s", m_feedList[feedName]->getID());
  
  // add article id
  params.addQueryItem("i", articleId);
  
  params.addQueryItem("async", "true");
  
  // add token
  params.addQueryItem("T", m_token);
  
  QByteArray paramsData;
  paramsData = params.encodedQuery();
  
  
  QNetworkRequest nreq = QNetworkRequest(request);
  nreq.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  
  QNetworkReply* reply = m_manager.post(setAuthHeader(nreq), paramsData);
  
  connect(reply, SIGNAL(finished()), SLOT(tagEdited()));
}

void Reader::tagEdited()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  
  // Old or none token
  if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400) {
    // update token
    // TODO: need to find out how to call editTag() again after receiving new token
    getToken();
    qDebug() << "Tag edited: " << reply->errorString() << ": token updated";
  } else if(reply->error()) {
    qDebug() << "Tag edited: " << reply->errorString();
  }
}




#include "reader.moc"

