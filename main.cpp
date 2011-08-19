#include <QtGui/QApplication>
#include "gready.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName("r_st");
    app.setApplicationName("gready");

    gready greadyApp;
    
    return app.exec();
}
