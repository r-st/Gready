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

class Tag;

class Reader : public QObject {
    Q_OBJECT

private:
    QString m_authKey;
    const QString m_apiUrl;
    QNetworkAccessManager m_manager;
    QSettings m_settings;
    QList<QNetworkReply*> m_replies;
    QList<Tag*> m_tagList;


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
     * @param reply response from server
     */
    void authenticated();
    void taglistFinished();
    
public slots:
    /**
     * Get list of tags
     */
    void getTags();
    void quit() {
        QCoreApplication::instance()->quit();
    }

};

#endif // READER_H
