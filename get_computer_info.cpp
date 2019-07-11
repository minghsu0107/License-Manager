#include <iostream> 
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <regex>
#include <direct.h>
#include <windows.h>
#include "get_computer_info.h"
#include "xxtea.h"
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

Extract::Extract() {}

Extract::Extract(string key): _key(key) {}

string Extract::get_key() { return _key; }

vector<string> Extract::execCmd(const char *cmd) {
    char buff[128] = {};
    vector<string> result;
    FILE *fp = _popen(cmd, "r");
    int cnt = 1;
    while (fgets(buff, 128, fp)) {
        if (cnt >= 2 && strlen(buff) > 2)
            result.push_back(buff);
        ++cnt;
    }
    _pclose(fp);

    for (int i = result.size() - 1; i >= 0; --i) {
        result[i] = result[i].substr(0, result[i].length() - 4);
    }
    return result;
}

vector<string> Extract::getHardDiskInfo() {
    return execCmd("wmic path win32_physicalmedia get SerialNumber");
}

vector<string> Extract::getMacInfo() {
    ifstream fp;
    string str;
    vector<string> mac;
    regex pattern("^([0-9A-Fa-f]{2}[:-]{1}){5}([0-9A-Fa-f]{2})$");

    system("ipconfig /all > mc.txt");
    fp.open("mc.txt"); 
    while (fp >> str) {
          if (regex_match(str, pattern))
              mac.push_back(str);
    }
    fp.close();
    remove("mc.txt");
    return mac;
}

json Extract::get_hardware_info() {
    json j;
    vector<string> hardDisk = getHardDiskInfo();
    vector<string> mac = getMacInfo();
    if (!hardDisk.size() && !mac.size())
        throw 101;

    j["mac"] = mac;
    j["hard_disk"] = hardDisk;
    return j;
}

json Extract::get_projector_id(json j, string projector_file) {
    ifstream fp(projector_file);
    if (!fp.is_open())
        throw 102;

    string str;
    fp >> str;
    fp.close();

    j["projector"] = str;
    return j;
}

json Extract::add_registration_time(json j) {
    time_t now = time(0);
    localtime(&now);
    j["time"] = now;
    return j;
}

string Extract::my_encryption(json j) {
    XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}

string Extract::get_user_name() {
    char uname[50];
    unsigned long size = sizeof(uname);
    GetUserName(uname, &size);
    return (string)uname;
}

string Extract::get_computer_name() {
    char name[MAX_COMPUTERNAME_LENGTH + 1];
    unsigned long size = sizeof(name);
    GetComputerName(name, &size);
    return (string)name;
}

int Extract::load(string projector_file) {
    json j;
    try {
        j = get_hardware_info();
        j = get_projector_id(j, projector_file);
    }
    catch (int error_code) {
            return error_code;
    }
    j = add_registration_time(j);
    string encryp = my_encryption(j);
#ifdef DEBUG
    ofstream o("get_info.json");
    o << j;
    o.close();
#endif
    _mkdir(".\\email_info"); 
    string name = get_computer_name() + "-" + get_user_name();
    ofstream out("email_info\\" + name + ".txt");
    out << encryp;
    out.close();
    return 0;
}