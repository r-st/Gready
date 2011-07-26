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

#include "tag.h"
#include "feed.h"

class Tag;

class Reader : public QObject {
    Q_OBJECT

private:
    QString m_authKey;
    const QString m_apiUrl;
    QNetworkAccessManager m_manager;
    QSettings m_settings;
    QList<QNetworkReply*> m_replies;
    QMap<QString, Tag*> m_tagList;


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
    
    void subscriptionFinished();
    
public slots:
    /**
     * Get list of tags
     */
    void getTags();
    
    void getAllFeeds();
    
    void quit() {
        QCoreApplication::instance()->quit();
    }

};

#endif // READER_H
