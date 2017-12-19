/*
 * egDb server sample console application
 *
 * Copyright (c) 2017 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include <thread>
#include <iostream>

// server sample & smoke test

using namespace std;

int main() // int argc, char *argv[]
{

    cout << "Number of hardware cores = "
              <<  std::thread::hardware_concurrency() << endl;
}
