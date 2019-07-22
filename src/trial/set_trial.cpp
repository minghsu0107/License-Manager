#include <string>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <sys/stat.h>
#include "set_trial.h"
#include "../encryption/xxtea.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;
typedef long long ll;

static string get_user_name() {
    char uname[50];
    unsigned long size = sizeof(uname);
    GetUserName(uname, &size);
    return (string)uname;
}

static bool SetHideAttribute(const TCHAR * lpName) { 
	DWORD dwResult = ::GetFileAttributes(lpName); 
	if (INVALID_FILE_ATTRIBUTES == dwResult)
   		return false; 
	if (!(FILE_ATTRIBUTE_HIDDEN & dwResult)) { 
   		if ( INVALID_FILE_ATTRIBUTES == ::SetFileAttributes(lpName, dwResult | FILE_ATTRIBUTE_HIDDEN)) 
       		return false; 
   		return true; 
	} 
	return true;
}

static bool file_exist(string file) {
	struct stat buf;
    if (stat(file.c_str(), &buf) == -1) {
        return false;
    }
    return true;
}

Trial::Trial(int trial_period, string key) {
	_trial_period = trial_period;
	_key = key;
	_trial_status = "C:\\Users\\" + get_user_name() + "\\.check";
}

int Trial::get_period() { return _trial_period; }

string Trial::get_status_file_name() { return _trial_status; }

string Trial::get_key() { return _key; }


string Trial::my_decryption(string str) {
	XXTEA en;
	return en.xxtea_decrypt(str, get_key());
}

string Trial::my_encryption(json j) {
	XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}

int Trial::initiate(ll now) {
	json j;
	j["start"] = now;
	j["low"] = now;
	j["high"] = now +  1ll * 86400 * get_period();
	string file = get_status_file_name();
	ofstream out(file);
	out << my_encryption(j);
	out.close();
	
	if (!SetHideAttribute(file.c_str()))
		return 1110;
	return 0;
}

void Trial::update(ll new_low) {
	string file = get_status_file_name();
	string tmp;
	ifstream in(file);
	in >> tmp;
	in.close();

	tmp = my_decryption(tmp);
	json j = json::parse(tmp);

	if (new_low > j["low"] && new_low <= j["high"]) {
		j["low"] = new_low;
		remove(file.c_str());
		ofstream out(file);
		out << my_encryption(j);
		out.close();
	}
}

int Trial::isExpired(ll now) {
	string file = get_status_file_name();
	string tmp;
	ifstream in(file);
	in >> tmp;
	in.close();

	tmp = my_decryption(tmp);
	json j = json::parse(tmp);
#ifdef DEBUG
	ofstream out("debug-trial.json");
	out << j.dump(4);
	out.close();
#endif	
	if (j["low"] <= now && now <= j["high"])
		return 0;
	return 1111;

}

int Trial::check() {
	time_t now;
	time(&now);
	if (!file_exist(get_status_file_name())) {
		int mes = initiate((ll)now);
		if (mes == 1110) return mes;
	}
	int flag = isExpired((ll)now);
	update((ll)now);
	return flag;
}