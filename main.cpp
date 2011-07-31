#include <QtGui/QApplication>
#include <QCoreApplication>
#include "gready.h"
#include "reader/reader.h"
#include <QTimer>


int main(int argc, char** argv)
{
    //QApplication app(argc, argv);
    QCoreApplication app(argc, argv);
    app.setOrganizationName("r_st");
    app.setApplicationName("gready");

    Reader reader;
    
    //gready foo;
    //foo.show();
    QTimer::singleShot(3000, &reader, SLOT(getTags()));
    QTimer::singleShot(5000, &reader, SLOT(getAllFeeds()));
    
    QTimer::singleShot(6000, &reader, SLOT(getArticlesFromFeed()));
    QTimer::singleShot(10000, &reader, SLOT(quit()));
    
    return app.exec();
}
