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


#include "feed.h"

Feed::Feed(Feed& oldFeed)
{
  m_id = oldFeed.m_id;
  m_name = oldFeed.m_name;
  m_parrentReader = oldFeed.m_parrentReader;
}

void Feed::getArticles(bool continueFeed)
{
  if(m_parrentReader == NULL) { 
    // TODO
    qDebug() << "No reader";
  } else {
    QString continuation = continueFeed ? m_continuation : "";
    m_parrentReader->getArticlesFromFeed(m_name, continuation);
    connect(m_parrentReader, SIGNAL(articlesFetchingDone(Feed*)), SIGNAL(articlesFetchingDone(Feed*)));
  }
}


#include "feed.moc"