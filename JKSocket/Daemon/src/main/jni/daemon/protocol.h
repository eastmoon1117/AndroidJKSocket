//
// Created by jared on 2018/2/10.
//

#ifndef JKSOCKET_PROTOCOL_H
#define JKSOCKET_PROTOCOL_H
#include <stdio.h>
#include <string>
#include "log.h"

using namespace std;

class Protocol {
private:
    char source;
    char destination;
    char cmd;
    char data_len;
    string datas;

public:
    void parse_protocol(string data);

    string combination_protocol(char source, char destination, char cmd, string data);

    char getSource() const;

    char getDestination() const;

    char getCmd() const;

    char getData_len() const;

    const string &getDatas() const;
};

#endif //JKSOCKET_PROTOCOL_H
