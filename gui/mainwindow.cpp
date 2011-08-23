/*
    Copyright (c) 2011 Robert Štětka <robert.stetka@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    QList<QString> headerLabels;
    headerLabels << tr("Title") << tr("Author") << tr("Date");
    ui->articlesTableView->setHeaderLabels(headerLabels);
    
    ui->feedsView->setHeaderLabel(tr("Name"));
  
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    
    //loadFeeds();
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFeeds()
{
  ui->feedsView->clear();
  
  QMap<QString, Tag*> tagList = m_reader->listTags();
  QMap<QString, Tag*>::iterator iter = tagList.begin();
  
  // load tags
  while(iter != tagList.end()) {
      QTreeWidgetItem* tag = new QTreeWidgetItem(ui->feedsView);
      tag->setText(0, iter.value()->getName());
      
      // load feeds
      QMap<QString, Feed*> feedList = iter.value()->getFeeds();
      QMap<QString, Feed*>::iterator feedIter = feedList.begin();
      
      while(feedIter != feedList.end()) {
        QTreeWidgetItem* feed = new QTreeWidgetItem(tag);
        feed->setText(0, feedIter.value()->getName());
        feedIter++;
      }
      //tag->addChildren(feeds);
      iter++;
  }
  
  // add feeds without tag
  QMap<QString, Feed*> feedList = m_reader->listFeeds();
  QMap<QString, Feed*>::iterator feedIterator= feedList.begin();
  
  while(feedIterator != feedList.end()) {
    if(!feedIterator.value()->hasCategory()) {
        QTreeWidgetItem* feed = new QTreeWidgetItem(ui->feedsView);
        feed->setText(0, feedIterator.value()->getName());
        
    }
    feedIterator++;
  }
      
  
  //ui->feedsView->insertTopLevelItems(0, tags);
  ui->feedsView->expandAll();
  connect(ui->feedsView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(loadArticlesFromFeed(QTreeWidgetItem*)));
}

void MainWindow::loadArticlesFromFeed(QTreeWidgetItem* item)
{
  Feed* feed = m_reader->listFeeds().value(item->text(0));
  if(feed == NULL) {
    // TODO
    qDebug() << "No such feed";
  } else {
    feed->getArticles();
    connect(m_reader, SIGNAL(articlesFetchingDone(Feed*)), SLOT(showArticlesFromFeed(Feed*)));
  }
}

void MainWindow::showArticlesFromFeed(Feed* feed)
{
  ui->articlesTableView->clear();
  
  QMap<QString, Article*> articlesList = feed->listArticles();
  QMap<QString, Article*>::iterator articlesIterator = articlesList.begin();
  
  
  while(articlesIterator != articlesList.end()) {
    QTreeWidgetItem* article = new QTreeWidgetItem(ui->articlesTableView);
    article->setText(0, articlesIterator.value()->getTitle());
    article->setText(1, articlesIterator.value()->getAuthor());
   
    /*
    QString publishedDate = articlesIterator.value()->getPublished().toString();
    article->setText(2, publishedDate);
    */
    article->setData(2, Qt::DisplayRole, QVariant(articlesIterator.value()->getPublished()));
    
    // hidden columns for further identification
    article->setText(3, articlesIterator.value()->getId());
    article->setText(4, feed->getName());
    
    articlesIterator++;
  }
  
  ui->articlesTableView->sortItems(2, Qt::DescendingOrder);
  connect(ui->articlesTableView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(loadArticleContent(QTreeWidgetItem*)));
}

void MainWindow::loadArticleContent(QTreeWidgetItem* item)
{
  Article* article = m_reader->listFeeds().value(item->text(4))->listArticles().value(item->text(3));
  
  if(article == NULL) {
    // TODO
    qDebug() << "No such article";
  } else {
    ui->articleView->setHtml(article->getContent());
  }
}




#include "mainwindow.moc"