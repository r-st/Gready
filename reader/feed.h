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


#ifndef FEED_H
#define FEED_H

#include <QtCore/QObject>
#include "reader.h"
#include "article.h"

class Reader;
class Article;

class Feed : public QObject
{
  Q_OBJECT
  
public:
    Feed(QString name, QString id, Reader* parrent = 0):m_name(name), m_id(id), m_parrentReader(parrent), m_unread(0) {};
    Feed(Reader* parrent = 0): m_parrentReader(parrent), m_unread(0) {};
    Feed(Feed &oldFeed);
    
    /**
     * Get name of the feed
     * @return name of the feed
     */
    QString getName() { return m_name; }
    
    /**
     * Get ID of the feed
     * @return ID
     */
    QString getID() { return m_id; }
    
    /**
     * Get continuation ID string
     * @return continuation ID
     */
    QString getContinuation() { return m_continuation; }
    
    /**
     * Set continuation ID string
     * @param continuation 
     */
    void setContinuation(QString continuation) { m_continuation = continuation; }
    
    /**
     * Sets hasCategory attribute
     * 
     * True if feed has some tag, false otherwise
     * @param hasCat
     */
    void setHasCategory(bool hasCat) { m_hasCategory = hasCat; }
    
    /**
     * Returns hasCategory attribute
     */
    bool hasCategory() { return m_hasCategory; }
    
    QMap<QString, Article*> listArticles() { return m_articles; }
    
    bool operator==(Feed &second) { if(m_id == second.m_id) { return true;} else { return false; } } /**
    
   * Sets number of unread articles
   * @param num number of unread articles
   */
  void setUnreadCount(unsigned int num) { m_unread = num; }
  
  /**
   * Returns number of unread articles
   * @return number of unread articles
   */
  unsigned int getUnreadCount() { return m_unread; }
    
public slots:
  /**
   * Fetch articles from feed
   */
  void getArticles(bool continueFeed = false);
  
  /**
   * Add article to feed
   * @param name id of the article
   * @param article pointer to Article object
   */
  void addArticle(QString articleId, Article* article) { m_articles.insert(articleId, article); }
    
private:
  // Tag label
  QString m_name;
  // Tag address
  QString m_id;
  // continuation ID string
  QString m_continuation;
  QMap<QString, Article*> m_articles;
  unsigned int m_unread;
  
  // true when feed has some tag, false when feed is top-level
  bool m_hasCategory;
  
  Reader* m_parrentReader;
  
signals:
  void articlesFetchingDone(Feed* feed);
};

#endif // FEED_H
