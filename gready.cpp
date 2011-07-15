#include "gready.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include "reader/reader.h"

gready::gready()
{
    QLabel* l = new QLabel( this );
    l->setText( "Hello World!" );
    setCentralWidget( l );
    
    //Reader foobar;
    //foobar.printID();
    //QLabel* ll = new QLabel( this );
    //ll->setText(foobar.printID());
    
    QAction* a = new QAction(this);
    a->setText( "Quit" );
    connect(a, SIGNAL(triggered()), SLOT(close()) );
    menuBar()->addMenu( "File" )->addAction( a );
}

gready::~gready()
{}

#include "gready.moc"
