#include <string>
#include <fstream>
#include <direct.h>
#include "generate_universal.h"
#include "xxtea.h"
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

Universal::Universal(string key, string version): _key(key), _version(version){}

string Universal::get_key() { return _key; }

string Universal::get_version() { return _version; }


string Universal::my_encryption(json j) {
    XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}

static void write_file(string res) {
    mkdir(".\\my_license"); 
    ofstream out("my_license\\license.txt");
    out << res;
    out.close();
}
void Universal::set_version(string &res) {
	res += "version=" + get_version();
}

void Universal::generate_universal_key() {
	json j;
	j["universal"] = true;
#ifdef DEBUG
	ofstream out("result.json");
	out << j.dump(4);
	out.close();
#endif
	string res = my_encryption(j);
	set_version(res);
	write_file(res);
}