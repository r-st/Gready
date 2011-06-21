#include <QtGui/QApplication>
#include "gready.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    gready foo;
    foo.show();
    return app.exec();
}
