#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QStandardItemModel>
#include <QFile>
#include <QTcpSocket>

#include "egDataNodesType.h"
#include "egGraphDatabase.h"

#include "project_form.h"
#include "funcblocks.h"

#include "ui_mainwindow.h"

// static QString objdb_global_string;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
/*
    DataObjects Projects;
    DataObjects Statuses;
    DataObjects Owners;
    DataObjects Funcblocks;
*/

    EgGraphDatabase graphDB;

    EgDataNodesType Projects;
    EgDataNodesType Statuses;
    EgDataNodesType Owners;
    EgDataNodesType Funcblocks;


    QStandardItemModel* model;

    // int FilterTest (QList<QVariant>& my_data); // filter callback
    // static void TestFunc(); //  { Owners.DClassName;};
    void refreshView();

    // QTcpSocket *tcpSocket;
    // QString currentFortune;
    // quint16 blockSize;

    EgRemoteConnect test_server;

protected:
//    void changeEvent(QEvent* e);

private:
    Ui::MainWindow* ui;
    int added_row; // new record auto-gen number

    QStandardItemModel* linked_model;
    // ObjectDatabase* my_database;
    // LinkData* ProjectStatusLink;
    // LinkData* ProjectOwnerLink;

    // QList<LinkData*> autolinks_list;

    ProjectForm* project_form;
    FuncBlocksForm* funcblocks_form;

    inline void InitProjectForm();
    void FillTestData();
    void TestLinks();

private slots:
    void on_initButton_clicked();
    void model_data_changed(const QModelIndex&, const QModelIndex&);
    void on_tableView_clicked(QModelIndex index);
    void on_deleteButton_clicked();
    void on_addButton_clicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_addProjectButton_clicked();
    void on_editProjectButton_clicked();

    // void GetServerData();
    // void displayError(QAbstractSocket::SocketError socketError);

    // void addProject();
    // void editProject();
};

#endif // MAINWINDOW_H
