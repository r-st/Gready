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
    QTimer::singleShot(4000, &reader, SLOT(getTags()));
    QTimer::singleShot(7000, &reader, SLOT(getAllFeeds()));
    QTimer::singleShot(20000, &reader, SLOT(quit()));
    
    return app.exec();
}
