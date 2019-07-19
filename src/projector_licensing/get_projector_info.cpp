#include <iostream> 
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <direct.h>
/*
#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
*/
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
    /*
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) return mac;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) return mac;

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    mac.push_back(string(ifr.ifr_hwaddr.sa_data));
                }
            }
        }
        else return mac;
    }
    */
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
    
    mkdir(output_dir_projector.c_str());
    string dir = output_dir_projector + "/email_info_projector";
    mkdir(dir.c_str()); 


    string name = getSerialNumber(serial_number_file);
    ofstream out(dir + "/" + name + ".txt");
    out << encryp;
    out.close();
    return 0;
}
/*
int main() {
    ProExtract e("1234567890abcdef");
    e.load("src/projector_licensing/projector_serial.txt", "output_projector");
}
*/