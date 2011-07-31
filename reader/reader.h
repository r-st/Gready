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
    QMap<QString, Tag*> m_tagList;
    QMap<QString, Feed*> m_feedList;
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
    
    void quit() {
        QCoreApplication::instance()->quit();
    }

};

#endif // READER_H
