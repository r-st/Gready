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
    //reader.getTags();
    reader.quit();
    
    //gready foo;
    //foo.show();
    QTimer::singleShot(4000, &reader, SLOT(getTags()));
    QTimer::singleShot(6000, &reader, SLOT(quit()));
    return app.exec();
}
