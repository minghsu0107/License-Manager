#include <string>
#include <fstream>
#include <sys/stat.h>
#include "generate_universal.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

Universal::Universal(string key, string version): _key(key), _version(version){}

string Universal::get_key() { return _key; }

string Universal::get_version() { return _version; }


string Universal::my_encryption(json j) {
    XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}

static void write_file(string res, string license_dir, bool isPC) {
    mkdir(license_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    string dir;
    if (isPC) 
        dir = license_dir + "/pc_license";
    else
        dir = license_dir + "/projector_license"; 
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 
    ofstream out(dir + "/license.txt");
    out << res;
    out.close();
}

void Universal::set_version(string &res) {
	res += "version=" + get_version();
}

void Universal::generate_universal_key(string license_dir, bool isPC) {
	json j;
	j["universal"] = true;
	j["tag"] = isPC? "pc": "projector";
#ifdef DEBUG
	ofstream out;
	if (isPC)
		out.open("debug_pc.json");
	else
		out.open("debug_projector.json");
	out << j.dump(4);
	out.close();
#endif
	string res = my_encryption(j);
	set_version(res);
	write_file(res, license_dir, isPC);
}
