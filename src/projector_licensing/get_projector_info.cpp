#include <string>
#include <cstdio>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "get_projector_info.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

ProExtract::ProExtract() {}

ProExtract::ProExtract(string key): _key(key) {}

string ProExtract::get_key() { return _key; }


string ProExtract::getSerialNumber(string serial_number_file) {
    ifstream in(serial_number_file);
    string str;
    in >> str;
    in.close();
    return str;
}

vector<string> ProExtract::getMacInfo() {
    vector<string> mac;
    string line;
    char ip_address[500];
    int hw_type, flags;
    char mac_address[500];
    char mask[500];
    char device[500];
	
    ifstream in("/proc/net/arp");
    getline(in, line);
    while (getline(in, line)) {
           sscanf(line.c_str(), "%s 0x%x 0x%x %s %s %s\n",
	           ip_address,
	           &hw_type,
	           &flags,
	           mac_address,
	           mask,
	           device);
	    mac.push_back(string(mac_address));
    }
    in.close();
    return mac;
}

json ProExtract::get_projector_info(string serial_number_file) {
    json j;
    string serial = getSerialNumber(serial_number_file);
    vector<string> mac = getMacInfo();
    if (!serial.length() && !mac.size())
        throw 102;

    j["mac"] = mac;
    j["serial_number"] = serial;
    return j;
}

string ProExtract::my_encryption(json j) {
    XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}


int ProExtract::load(string serial_number_file, string output_dir_projector) {
    json j;
    try {
        j = get_projector_info(serial_number_file);
    }
    catch (int error_code) {
            return error_code;
    }
    string encryp = my_encryption(j);
    //cout <<j;
    
    mkdir(output_dir_projector.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    string dir = output_dir_projector + "/email_info_projector";
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 


    string name = getSerialNumber(serial_number_file);
    ofstream out(dir + "/" + name + ".txt");
    out << encryp;
    out.close();
    return 0;
}
