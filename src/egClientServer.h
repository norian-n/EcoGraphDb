/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

// client/server related constants

// #include <qglobal.h>

typedef uint8_t  CommandIdType;    // client/server operation codes

const uint16_t server_port = 5777;  // hardcoded default server port

const int egServerTimeout = 10000;  // 10 sec

const CommandIdType opcode_send_odb_id     = 0x01;     // send odb_id to server
const CommandIdType opcode_get_odb_id      = 0x02;     // get odb id from server by object class name

const CommandIdType opcode_store_metainfo  = 0x03;     // store fields descriptions on server
const CommandIdType opcode_load_metainfo   = 0x04;     // load ones from server

const CommandIdType opcode_store_data      = 0x05;     // append new data obj to file on server
const CommandIdType opcode_load_all_data   = 0x06;     // load all data from server

const CommandIdType opcode_load_selected_data   = 0x07;     // set filter plugin on server and load data


#endif // CLIENTSERVER_H
