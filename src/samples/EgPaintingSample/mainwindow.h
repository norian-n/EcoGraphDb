#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <GraphSceneForm.h>

#include <QFile>
#include <QDir>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

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

    GraphSceneForm *sceneForm = nullptr;

    EgGraphDatabase graphDB;

    EgDataNodesType nodes;
    EgDataNodesType images;

    EgLinkType links;

    void AddImages();

    void cleanUpFiles();

};

#endif // MAINWINDOW_H
