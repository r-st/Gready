#include <QtGui/QApplication>
#include <QCoreApplication>
#include "gready.h"
#include "reader/reader.h"


int main(int argc, char** argv)
{
    //QApplication app(argc, argv);
    QCoreApplication app(argc, argv);
    app.setOrganizationName("r_st");
    app.setApplicationName("gready");
    
    Reader reader;
    reader.printID();
    reader.quit();
    
    //gready foo;
    //foo.show();
    return app.exec();
}
