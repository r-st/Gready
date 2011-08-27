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

#ifndef READER_H
#define READER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QSignalMapper>
#include <QMimeData>

#include "tag.h"
#include "feed.h"
#include "article.h"

class Tag;

class Reader : public QObject {
    Q_OBJECT

private:
    QString m_authKey;
    const QString m_apiUrl;
    const QString m_atomUrl;
    QNetworkAccessManager m_manager;
    QSettings m_settings;
    QList<QNetworkReply*> m_replies;
    
    // list of tags mapped by tag title
    QMap<QString, Tag*> m_tagList;
    // list of tags mapped by it's id
    QMap<QString, Tag*> m_tagListByID;
    
    // list of feeds mapped by feed title
    QMap<QString, Feed*> m_feedList;
    // list of feeds mapped by id's id
    QMap<QString, Feed*> m_feedListByID;
    
    QSignalMapper* m_signalMapper;


    /**
     * Get authentication key from Google
     */
    void getID();

    /**
     * Set header with authentication key
     */
    QNetworkRequest setAuthHeader(QNetworkRequest req);

public:
    Reader();

private slots:
    /**
     * Process SID retrieved from getID
     */
    void authenticated();
    
    /**
     * Process fetched tags
     */
    void taglistFinished();
    
    /**
     * Process fetched feeds
     */
    void feedsFetched();
    
    /**
     * Process fetched articles from feed
     */
    void articlesFromFeedFinished(QString feedName);
    
    /**
     * Process number of unread articles
     */
    void unreadFinished();
    
public slots:
    /**
     * Get list of tags
     */
    void getTags();
    
    /**
     * Get list of feeds
     */
    void getAllFeeds();
    
    /**
     * Fetch articles from given feed
     * @param feedName name of the feed
     */
    void getArticlesFromFeed(QString feedName);
    
    /**
     * Returns list of tags
     * @return list of tags
     */
    QMap<QString, Tag*> listTags() { return m_tagList; }
    
    /**
     * Returns list of feeds
     * @return list of feeds
     */
    QMap<QString, Feed*> listFeeds() { return m_feedList; }
    
    /**
     * Fetches number of unread articles
     */
    void getUnreadCount();
    
signals:
  void authenticationDone();
  void tagsFetchingDone();
  void feedsFetchingDone();
  void articlesFetchingDone(Feed* feed);
  void unreadCountDone();

};

#endif // READER_H
