/* File: Sniffer.h
 *************************************/

#include <tins/tins.h>
#include <tins/utils.h>
#include <iostream>
#include <fstream>
 
void initSniffer(const std::string interface);
bool callback(const Tins::PDU &pdu);