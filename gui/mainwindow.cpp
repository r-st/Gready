#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmap.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    
    //loadFeeds();
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFeeds()
{
  
  QList<QTreeWidgetItem*> tags;
  QMap<QString, Tag*> tagList = m_reader->listTags();
  QMap<QString, Tag*>::iterator iter = tagList.begin();
  while(iter != tagList.end()) {
    QTreeWidgetItem* tag = new QTreeWidgetItem;
    tag->setText(0, iter.value()->getName());
    tags.append(tag);
    iter++;
  }
  
  ui->feedsView->insertTopLevelItems(0, tags);
  ui->feedsView->setHeaderLabel(tr("Name"));
  ui->feedsView->expandAll();
}


#include "mainwindow.moc"