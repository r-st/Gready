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


#ifndef ARTICLE_H
#define ARTICLE_H

#include <QtCore/QObject>
#include <QDateTime>
#include <QUrl>
#include <QMimeData>


class Article : public QObject
{
    Q_OBJECT;
public:
    Article(QString title, QString author, QString articleId, QUrl articleUrl, QMimeData &articleContent,
            QDateTime published = QDateTime::currentDateTime(), bool isRead = false, bool isStarred = false, bool isShared = false):
            m_title(title),
            m_author(author),
            m_articleId(articleId),
            m_articleUrl(articleUrl),
            m_published(published),
            m_isRead(isRead),
            m_isStarred(isStarred),
            m_isShared(isShared)
    {
      m_articleContent.setHtml(articleContent.html());
    };
    
    QString getTitle() { return m_title; }
    QString getAuthor() { return m_author; }
    QDateTime getPublished() { return m_published; }
    QUrl getUrl() { return m_articleUrl; }
    QString getContent() { return m_articleContent.html(); }
    QString getId() { return m_articleId; }
    bool isRead() { return m_isRead; }
    bool isShared() { return m_isShared; }
    bool isStarred() { return m_isStarred; }
    
private:
    QDateTime m_published;
    QUrl m_articleUrl;
    QString m_author;
    QMimeData m_articleContent;
    bool m_isRead;
    bool m_isStarred;
    bool m_isShared;
    QString m_articleId;
    QString m_title;
};

#endif // ARTICLE_H
