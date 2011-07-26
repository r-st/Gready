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


class Feed : public QObject
{
  Q_OBJECT
  
public:
    Feed(QString name, QString id):m_name(name), m_id(id) {};
    Feed() {};
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
    
private:
  QString m_name;
  QString m_id;
};

#endif // FEED_H
