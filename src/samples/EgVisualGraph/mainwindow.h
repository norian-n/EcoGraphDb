#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <EgGraphForm.h>

#include <QFile>
#include <QDir>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

#include <QObject>
/*
class myEventFilter: public QObject {
  Q_OBJECT
public:
  myEventFilter() {}
  ~myEventFilter() {
  }
protected:
  bool eventFilter(QObject* object, QEvent* event) {
    if(event->type() == QEvent::DragEnter)
    {
      qDebug() << "Filter Drag enter ";
      // event->setAccepted(true);
    }

    if(event->type() == QEvent::Drop)
    {
      qDebug() << "Filter Drop ";
    }

    return false;
  }
};
*/
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_initButton_clicked();

    void on_sceneButton_clicked();

private:
    Ui::MainWindow *ui;

    EgGraphForm *sceneForm = nullptr;

    EgGraphDatabase graphDB;

    EgDataNodesType nodes;
    EgDataNodesType images;

    EgLinkType links;

    void AddImages();

    void cleanUpFiles();

};

#endif // MAINWINDOW_H
