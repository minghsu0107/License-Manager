#include <iostream> 
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <regex>
#include <direct.h>
#include <winsock2.h>
#include <windows.h>
#include "get_pc_info.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
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

json Extract::add_os_info(json j) {
    OSVERSIONINFO ver = { sizeof(OSVERSIONINFO) };
    GetVersionEx(&ver);
    string os_name;
    if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 0)
        os_name = "Windows_2000";
    else if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 1)
        os_name = "Windows_XP";
    else if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 0)
        os_name = "Windows_2003";
    else if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 2)
        os_name = "windows_vista";
    else if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 1)
        os_name = "windows_7";
    else if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 2)
        os_name = "windows_10";
    
    os_name += "_" + to_string(ver.dwMajorVersion) + "." + to_string(ver.dwMinorVersion);
    j["os_name"] = os_name;
    return j;
}

int Extract::load(string output_dir_pc) {
    json j;
    try {
        j = get_hardware_info();
    }
    catch (int error_code) {
            return error_code;
    }
    j = add_os_info(j);
    string encryp = my_encryption(j);
    
    mkdir(output_dir_pc.c_str());
    string dir = output_dir_pc + "/email_info_pc";
    mkdir(dir.c_str()); 

    string name = get_computer_name() + "-" + get_user_name();
    transform(name.begin(), name.end(), name.begin(),
    [](unsigned char c){ return std::tolower(c); });

    ofstream out(dir + "/" + name + ".txt");
    out << encryp;
    out.close();
    return 0;
}
