#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include <assert.h>
#include <direct.h>
#include "license_validation.h"
#include "get_computer_info.h"
#include "xxtea.h"
#include "json.hpp"
using namespace std;
using json=nlohmann::json;
typedef long long int ll;

Validation::Validation(string key): _key(key) {}

string Validation::get_key() { return _key; }

bool Validation::isExpired(string name, json info) {
	time_t now = time(0);
    localtime(&now);
	auto t = info[name]["time"];
	auto span_day = info[name]["expiration"]["time_span"];

	ll span_sec = 86400 * (ll)span_day;
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
	j["service_status"] = info[name]["service_status"];
	j["registration_time"] = info[name]["time"];
	j["time_span"] = info[name]["expiration"]["time_span"];
	j["projector"] = info[name]["projector"];
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

int Validation::judge(json info) {
	Extract e;
	json fingerprint;
	try {
	    fingerprint = e.get_hardware_info();
	}
	catch (int error_code){
		return error_code;
	}
    string name = e.get_computer_name() + "-" + e.get_user_name();
    
	if (match(name, info, fingerprint)) {
		if (!isExpired(name, info)) {
			_mkdir(".\\output"); 
			dump_json(name, info, "output\\out.json");
			return 0;
		}
		else return 1002;
	}
	return 1003;
}

int Validation::validate() {
	ifstream in("my_license\\license.txt");
	string encryp;
	getline(in, encryp);

	string decryp = my_decryption(encryp);
	if (!IsRightFormat(decryp)) return 1001;

	auto info = parser(decryp);
    return judge(info);
}
