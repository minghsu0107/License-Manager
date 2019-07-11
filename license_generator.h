#ifndef LICENSE_GENERATOR_H
#define LICENSE_GENERATOR_H
#include <string>
#include "json.hpp"
using namespace std;
using json=nlohmann::json;
class Generator {
public:
	Generator(string key, string version);
	string get_key();
	string get_version();
	int generate(string source_dir, string func_dir);
private:
	string _key;
	string _version;
	pair<json, json> get_variables(string func_file);
	string my_encryption(json j);
	string my_decryption(string str);
	void update(json &j, string decryp, string func_dir, string file);
	void set_version(string &res);
};
#endif