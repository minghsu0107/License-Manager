#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <assert.h>
#include <direct.h>
#include "license_validation_pc.h"
#include "get_pc_info.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;
typedef long long int ll;

Validation::Validation(string key): _key(key) {}

string Validation::get_key() { return _key; }

bool Validation::isExpired(string name, json info) {
	time_t now;
	time(&now);
	auto t = info[name]["registration_time"];
	auto span_day = info[name]["expiration"]["time_span"];

	ll span_sec = 1ll * 86400 * (ll)span_day;
	if ((ll)t + span_sec <= (ll)now) return true;
	return false;
}

bool Validation::match(string name, json info, json fingerprint) {
	json j1 = info[name]["mac"], j2 = fingerprint["mac"];
	assert(j1.size() == j2.size());
	for (int i = 0; i < j1.size(); ++i)
		if (j1[i] != j2[i]) return false;
	
	j1 = info[name]["hard_disk"], j2 = fingerprint["hard_disk"];
	for (int i = 0; i < j1.size(); ++i)
		if (j1[i] != j2[i]) return false;
	return true;
}

void Validation::dump_json(string name, json info, string file) {
	json j;
	j["universal"] = false;
	j["service_status"] = info[name]["service_status"];
	j["registration_time"] = info[name]["registration_time"];
	j["time_span"] = info[name]["expiration"]["time_span"];

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

json Validation::parser(string str) { return json::parse(str); }

string Validation::my_decryption(string str) {
	XXTEA en;
	return en.xxtea_decrypt(str.substr(0, str.length() - 13), get_key());
}

void Validation::write_universal(string file) {
	json j;
	j["universal"] = true;
	ofstream out(file);
	out << j.dump(4);
	out.close();
}

int Validation::judge(json info, string output_dir) {
	if (info["universal"]) {
		mkdir(output_dir.c_str());
		string dir = output_dir + "/final_result";
		mkdir(dir.c_str()); 
		write_universal(dir + "/result.json");
		return 10;
	}

	Extract e;
	json fingerprint;
	try {
	    fingerprint = e.get_hardware_info();
	}
	catch (int error_code){
		return error_code;
	}
	string name = e.get_computer_name() + "-" + e.get_user_name();
	transform(name.begin(), name.end(), name.begin(),
		  [](unsigned char c){ return std::tolower(c); });
    
	if (match(name, info, fingerprint)) {
		if (!isExpired(name, info)) {
			mkdir(output_dir.c_str());
			string dir = output_dir + "/final_result";
			mkdir(dir.c_str()); 
			dump_json(name, info, dir + "/result.json");
			return 0;
		}
		else return 1002;
	}
	return 1003;
}

int Validation::validate(string license_dir, string output_dir) {
	ifstream in(license_dir + "/pc_license/license.txt");
	string encryp;
	getline(in, encryp);

	string decryp = my_decryption(encryp);
	
	if (!IsRightFormat(decryp)) return 1001;

	auto info = parser(decryp);
	return judge(info, output_dir);
}
