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

MainWindow::MainWindow(Reader* reader, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_reader(reader)
{
    ui->setupUi(this);
    
    QList<QString> articleHeaderLabels;
    articleHeaderLabels << tr("Title") << tr("Author") << tr("Date");
    ui->articlesTableView->setHeaderLabels(articleHeaderLabels);
    
    QList<QString> feedHeaderLabels;
    feedHeaderLabels << tr("Name") << tr("Unread");
    ui->feedsView->setHeaderLabels(feedHeaderLabels);
  
    // close window
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    
    // open settings window
    connect(ui->actionSettings, SIGNAL(triggered(bool)), SLOT(showSettingsWindow()));
    
    // fetch list of articles on click
    connect(ui->feedsView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(loadArticlesFromFeed(QTreeWidgetItem*)));
    
    // show list articles afer fetching them
    connect(m_reader, SIGNAL(articlesFetchingDone(Feed*)), SLOT(showArticlesFromFeed(Feed*)));
    
    // show content of the article on click
    connect(ui->articlesTableView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(loadArticleContent(QTreeWidgetItem*)));
    
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
      
      // set font to bold when there are unread articles
      if(iter.value()->getUnreadCount() > 0) {
        QFont font;
        font.setBold(true);
        tag->setFont(1, font);
      }
      tag->setText(1, QString::number(iter.value()->getUnreadCount()));
      
      // load feeds
      QMap<QString, Feed*> feedList = iter.value()->getFeeds();
      QMap<QString, Feed*>::iterator feedIter = feedList.begin();
      
      while(feedIter != feedList.end()) {
        QTreeWidgetItem* feed = new QTreeWidgetItem(tag);
        feed->setText(0, feedIter.value()->getName());
        
        if(feedIter.value()->getUnreadCount() > 0) {
          QFont font;
          font.setBold(true);
          feed->setFont(1, font);
        }
        feed->setText(1, QString::number(feedIter.value()->getUnreadCount()));
          
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
        if(feedIterator.value()->getUnreadCount() > 0) {
          QFont font;
          font.setBold(true);
          feed->setFont(1, font);
        }
        feed->setText(1, QString::number(feedIterator.value()->getUnreadCount()));
        
    }
    feedIterator++;
  }
      
  
  //ui->feedsView->insertTopLevelItems(0, tags);
  ui->feedsView->expandAll();
}

void MainWindow::loadArticlesFromFeed(QTreeWidgetItem* item)
{
  Feed* feed = m_reader->listFeeds().value(item->text(0));
  if(feed == NULL) {
    // TODO
    qDebug() << "No such feed";
  } else {
    feed->getArticles();
  }
}

void MainWindow::showArticlesFromFeed(Feed* feed)
{
  ui->articlesTableView->clear();
  
  QMap<QString, Article*> articlesList = feed->listArticles();
  QMap<QString, Article*>::iterator articlesIterator = articlesList.begin();
  
  
  while(articlesIterator != articlesList.end()) {
    QTreeWidgetItem* article = new QTreeWidgetItem(ui->articlesTableView);
    
    // set bold font for unread articles
    if(!articlesIterator.value()->isRead()) { 
      QFont font;
      font.setBold(true);
      
      article->setFont(0, font);
      article->setFont(1, font);
      article->setFont(2, font);
    }
    
    article->setText(0, articlesIterator.value()->getTitle());
    article->setText(1, articlesIterator.value()->getAuthor());
   
    article->setData(2, Qt::DisplayRole, QVariant(articlesIterator.value()->getPublished()));
    
    // hidden columns for further identification
    article->setText(3, articlesIterator.value()->getId());
    article->setText(4, feed->getName());
    
    articlesIterator++;
  }
  
  ui->articlesTableView->sortItems(2, Qt::DescendingOrder);
}

void MainWindow::loadArticleContent(QTreeWidgetItem* item)
{
  Article* article = m_reader->listFeeds().value(item->text(4))->listArticles().value(item->text(3));
  
  if(article == NULL) {
    // TODO
    qDebug() << "No such article";
  } else {
    // set article as read
    if(!article->isRead()) {
      m_reader->editTag(article->getId(), item->text(4), Reader::Add, Reader::Read);
    
      QFont font;
      font.setBold(false);
      
      // unset bold for article
      item->setFont(0, font);
      item->setFont(1, font);
      item->setFont(2, font);
      
      // decrease number of unread articles from feed
      Feed* feed = m_reader->listFeeds().value(ui->feedsView->currentItem()->text(0));
      feed->setUnreadCount(feed->getUnreadCount() - 1);
      if(feed->getUnreadCount() == 0) {
        ui->feedsView->currentItem()->setFont(1, font);
      }
      ui->feedsView->currentItem()->setText(1, QString::number(feed->getUnreadCount()));
      
      if(ui->feedsView->currentItem()->parent()) {
        Tag* tag = m_reader->listTags().value(ui->feedsView->currentItem()->parent()->text(0));
        tag->setUnreadCount(tag->getUnreadCount() - 1);
        
        if(tag->getUnreadCount() == 0) {
          ui->feedsView->currentItem()->parent()->setFont(1, font);
        }
        ui->feedsView->currentItem()->parent()->setText(1, QString::number(tag->getUnreadCount()));
      }
    }
    ui->articleView->setHtml(article->getContent());
  }
}

void MainWindow::showSettingsWindow()
{
  SettingsWindow settingsWindow;
  settingsWindow.exec();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    if(ui->articlesTableView->currentItem()) {
      Article* article;
      if((article = m_reader->listFeeds().value(ui->articlesTableView->currentItem()->text(4))->listArticles().value(ui->articlesTableView->currentItem()->text(3))) != NULL) {
        QDesktopServices desktop;
        desktop.openUrl(article->getUrl());
      }
    }
  } else {
    QMainWindow::keyPressEvent(event);
  }
}





#include "mainwindow.moc"