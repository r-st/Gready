#include "mainwindow.h"
#include "ui_mainwindow.h"

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
  ui->feedsView->clear();
  ui->feedsView->setHeaderLabel(tr("Name"));
  
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
  QList<QString> headerLabels;
  headerLabels << tr("Title") << tr("Author") << tr("Date");
  ui->articlesTableView->setHeaderLabels(headerLabels);
  
  QMap<QString, Article*> articlesList = feed->listArticles();
  QMap<QString, Article*>::iterator articlesIterator = articlesList.begin();
  
  
  while(articlesIterator != articlesList.end()) {
    QTreeWidgetItem* article = new QTreeWidgetItem(ui->articlesTableView);
    article->setText(0, articlesIterator.value()->getTitle());
    article->setText(1, articlesIterator.value()->getAuthor());
    
    QString publishedDate = articlesIterator.value()->getPublished().toString();
    article->setText(2, publishedDate);
    article->setText(3, articlesIterator.value()->getId());
    
    articlesIterator++;
  }
  
}





#include "mainwindow.moc"