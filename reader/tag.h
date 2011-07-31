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


#ifndef TAG_H
#define TAG_H

#include <QCoreApplication>
#include "reader.h"
#include "feed.h"

class Reader;
class Feed;

class Tag : public QObject
{
  Q_OBJECT
  
public:
  enum type {
    Starred,
    Shared,
    Following,
    Label,
    StateUnknown 
  };
  Tag() {}
  Tag(QString name, QString tagLabel, type tagType = Label, Reader* parrent = NULL): m_tagName(name), m_tagLabel(tagLabel), m_tagType(tagType), m_parrent(parrent) {};
  
  /**
   * Returns name of the tag
   * @return name of the tag
   */
  QString getName() { return m_tagName;}
  
  /**
   * Returns full name of the tag
   * @return full name of the tag
   */
  QString getTagLabel() { return m_tagLabel;}
  
  /**
   * Returns type of the tag
   * @return type of the tag
   */
  type getType() { return m_tagType; }
  
  /**
   * Sets name of the tag
   * @param name name of the tag
   */
  void setName(QString name);
  
  /**
   * Sets full name of the tag
   * @param tagLabel full name of the tag
   */
  void setTagLabel(QString tagLabel);
  
  /**
   * Sets type of the tag
   * @param tagType type of the tag
   */
  void setType(type tagType);
  
  /**
   * Sets parrent feed reader of the tag
   * @param parrent feed reader
   */
  void setParrent(Reader* parrent);
  
  /**
   * Adds new feed to tag
   * @param feed feed added to tag
   */
  void addFeed(Feed* feed);
  
private: 
  QString m_tagName;
  QString m_tagLabel;
  type m_tagType;
  Reader* m_parrent;
  QMap<QString, Feed*> m_feeds; 
};

#endif // TAG_H
