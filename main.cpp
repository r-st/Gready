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
    QTimer::singleShot(5000, &reader, SLOT(getTags()));
    QTimer::singleShot(11000, &reader, SLOT(quit()));
    
    return app.exec();
}
