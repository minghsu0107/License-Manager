#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <sys/stat.h>
#include "license_validation_projector.h"
#include "get_projector_info.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;
typedef long long int ll;

ProValidation::ProValidation(string key): _key(key) {}

string ProValidation::get_key() { return _key; }

bool ProValidation::match(string name, json info, json fingerprint) {
	json j1 = info[name]["mac"], j2 = fingerprint["mac"];
	assert(j1.size() == j2.size());
	for (int i = 0; i < j1.size(); ++i)
		if (j1[i] != j2[i]) return false;

	j1 = info[name]["serial_number"], j2 = fingerprint["serial_number"];
	if (j1 != j2) return false;
	return true;
}

void ProValidation::dump_json(string name, json info, string file) {
	json j;
	j["universal"] = false;
	j["service_status"] = info[name]["service_status"];

	ofstream out(file);
	out << j.dump(4);
	out.close();
}

static bool IsRightFormat(string strData) {
	if (strData[0] != '{')
		return false;
 
	int num = 0;
	for (int i = 0; i < strData.length(); ++i) {
		if (strData[i] == '{')
			++num;
		else if (strData[i] == '}')
			--num;
 	}
 	
 	if (!num) return true;
 	return false;
}

json ProValidation::parser(string str) { return json::parse(str); }

string ProValidation::my_decryption(string str) {
	XXTEA en;
	return en.xxtea_decrypt(str.substr(0, str.length() - 13), get_key());
}

void ProValidation::write_universal(string file) {
	json j;
	j["universal"] = true;
	ofstream out(file);
	out << j.dump(4);
	out.close();
}

int ProValidation::judge(json info, string output_dir,
	                     string serial_number_file) {
	if (info["universal"]) {
		mkdir(output_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		string dir = output_dir + "/final_result";
		mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 
		write_universal(dir + "/result.json");
		return 11;
	}

	ProExtract e;
	json fingerprint;
	try {
	    fingerprint = e.get_projector_info(serial_number_file);
	}
	catch (int error_code){
		return error_code;
	}
	string name = e.getSerialNumber(serial_number_file);
	
	if (match(name, info, fingerprint)) {
		mkdir(output_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		string dir = output_dir + "/final_result";
		mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 
		dump_json(name, info, dir + "/result.json");
		return 0;
	}
	return 1005;
}

int ProValidation::validate(string license_dir, string output_dir,
	                        string serial_number_file) {
	ifstream in(license_dir + "/projector_license/license.txt");
	string encryp;
	getline(in, encryp);
	
	string decryp = my_decryption(encryp);
	if (!IsRightFormat(decryp)) return 1004;

	auto info = parser(decryp);
	return judge(info, output_dir, serial_number_file);
}