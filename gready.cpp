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

#include "gready.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

gready::gready()
{
    Reader* reader = new Reader;
    connect(reader, SIGNAL(authenticationDone()), reader, SLOT(getTags()));
    MainWindow* main = new MainWindow;
    connect(reader, SIGNAL(tagsFetchingDone()), reader, SLOT(getAllFeeds()));
    connect(reader,SIGNAL(feedsFetchingDone()), reader, SLOT(getUnreadCount()));
    connect(reader, SIGNAL(unreadCountDone()), main, SLOT(loadFeeds()));
    
    main->setReader(reader);
    main->show();
}

gready::~gready()
{}

#include "gready.moc"
