#ifndef SET_TRIAL_H
#define SET_TRIAL_H
#include <string>
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

class Trial {
public:
	Trial(int trial_period, string key);
	int get_period();
	string get_status_file_name();
	string get_key();
	int check();

private:
	int _trial_period;
	string _trial_status;
	string _key;
	int initiate(long long now);
	void update(long long new_low);
	int isExpired(long long now);
	string my_decryption(string str);
	string my_encryption(json j);
};
#endif