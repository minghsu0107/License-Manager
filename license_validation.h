#ifndef LICENSE_VALIDATION_H
#define LICENSE_GENERATOR_H
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

class Validation {
public:
	Validation(string key);
	string get_key();
	int validate();
private:
	string _key;
	string my_decryption(string str);
	json parser(string str);
	bool isExpired(string name, json info);
	bool match(string name, json info, json fingerprint);
	void dump_json(string name, json info, string file);
	int judge(json info);
};
#endif
