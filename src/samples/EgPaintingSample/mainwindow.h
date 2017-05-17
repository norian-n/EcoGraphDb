#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <GraphSceneForm.h>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_initButton_clicked();

    void on_sceneButton_clicked();

private:
    Ui::MainWindow *ui;

    GraphSceneForm *sceneForm = nullptr;

    EgGraphDatabase graphDB;
    EgDataNodesType nodes;

};

#endif // MAINWINDOW_H
