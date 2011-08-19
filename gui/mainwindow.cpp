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
  
  // load tags
  while(iter != tagList.end()) {
      QTreeWidgetItem* tag = new QTreeWidgetItem();
      tag->setText(0, iter.value()->getName());
      
      // load feeds
      QList<QTreeWidgetItem*> feeds;
      QMap<QString, Feed*> feedList = iter.value()->getFeeds();
      QMap<QString, Feed*>::iterator feedIter = feedList.begin();
      
      while(feedIter != feedList.end()) {
        QTreeWidgetItem* feed = new QTreeWidgetItem(tag);
        feed->setText(0, feedIter.value()->getName());
        feeds.append(feeds);
        feedIter++;
      }
      tag->addChildren(feeds);
      tags.append(tag);
      iter++;
  }
  
  // add feeds without tag
  QMap<QString, Feed*> feedList = m_reader->listFeeds();
  QMap<QString, Feed*>::iterator feedIterator= feedList.begin();
  
  while(feedIterator != feedList.end()) {
    if(!feedIterator.value()->hasCategory()) {
        QTreeWidgetItem* feed = new QTreeWidgetItem;
        feed->setText(0, feedIterator.value()->getName());
        
        tags.append(feed);
    }
    feedIterator++;
  }
      
  
  
  ui->feedsView->insertTopLevelItems(0, tags);
  ui->feedsView->setHeaderLabel(tr("Name"));
  ui->feedsView->expandAll();
}


#include "mainwindow.moc"