//
// Created by jared on 2018/2/10.
//

#include <string>
#include "protocol.h"

//src dest cmd len data
void Protocol::parse_protocol(string data) {
    if(data.size() < 4) return;
    source = data[0]-'0';
    destination = data[1]-'0';
    cmd = data[2]-'0';
    data_len = data[3]-'0';
    datas = data.substr(4);
}

string Protocol::combination_protocol(char source, char destination, char cmd, string data) {
    LOG_D("source:%d, dest:%d, cmd:%d, data:%s", source, destination, cmd, data.data());
    string send_data = "";
    send_data += source+'0';
    send_data += destination+'0';
    send_data += cmd + '0';
    send_data += data.size()+'0';
    send_data += data;

    LOG_D("send_data: %s", send_data.data());

    return send_data;
}

char Protocol::getSource() const {
    return source;
}

char Protocol::getDestination() const {
    return destination;
}

char Protocol::getCmd() const {
    return cmd;
}

const string &Protocol::getDatas() const {
    return datas;
}
