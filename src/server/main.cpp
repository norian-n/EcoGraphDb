/*
 * egDb server sample console application
 *
 * Copyright (c) 2017 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include <QCoreApplication>
#include <QtCore>

// #include <thread>
#include <iostream>

#include "egServerEngine.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EgServerEngine serverEngine; // QWidget *parent

    serverEngine.run();

    cout << "Press Ctrl-C to quit ..." << endl;
    // getchar();

    return a.exec();
}
