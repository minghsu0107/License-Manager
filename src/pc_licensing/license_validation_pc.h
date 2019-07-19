#ifndef LICENSE_VALIDATION_PC_H
#define LICENSE_VALIDATION_PC_H
#include <string>
#include <vector>
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

class Validation {
public:
	Validation(string key);
	string get_key();
	int validate(string license_dir, string output_dir);
private:
	string _key;
	string my_decryption(string str);
	json parser(string str);
	bool isExpired(string name, json info);
	bool match(string name, json info, json fingerprint);
	void dump_json(string name, json info, string file);
	void write_universal(string file);
	int judge(json info, string output_dir);
};
#endif
