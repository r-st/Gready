#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../reader/reader.h"
#include "../reader/tag.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void setReader(Reader* reader) { m_reader = reader; }

private:
    Ui::MainWindow *ui;
    Reader* m_reader;
    
    
public slots:
    void loadFeeds();
};

#endif // MAINWINDOW_H
