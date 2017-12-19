#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

// client/server related constants

// #include <qglobal.h>

typedef uint8_t  command_id_type;    // client/server operation codes

const uint16_t server_port = 5777;   // hardcoded default server port

const command_id_type opcode_send_odb_id     = 0x01;     // send odb_id to server
const command_id_type opcode_get_odb_id      = 0x02;     // get odb id from server by object class name

const command_id_type opcode_store_metainfo  = 0x03;     // store fields descriptions on server
const command_id_type opcode_load_metainfo   = 0x04;     // load ones from server

const command_id_type opcode_append_dataobj  = 0x05;     // append new data obj to file on server
const command_id_type opcode_delete_dataobj  = 0x06;     // mark data obj as deleted on server
const command_id_type opcode_update_dataobj  = 0x07;     // mark data obj as deleted on server

const command_id_type opcode_load_data       = 0x08;     // load all data from server
const command_id_type opcode_load_filtered   = 0x09;     // set filter plugin on server and load data


#endif // CLIENTSERVER_H
