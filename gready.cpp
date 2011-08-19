#include "gready.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

gready::gready()
{
    Reader* reader = new Reader;
    reader->getTags();
    connect(reader, SIGNAL(authenticationDone()), reader, SLOT(getTags()));
    MainWindow* main = new MainWindow;
    connect(reader, SIGNAL(tagsFetched()), main, SLOT(loadFeeds()));
    main->setReader(reader);
    main->show();
}

gready::~gready()
{}

#include "gready.moc"
