#ifndef LICENSE_GENERATOR_H
#define LICENSE_GENERATOR_H
#include <string>
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;
class Generator {
public:
	Generator(string key, string version);
	string get_key();
	string get_version();
	int generate(string source_dir, string func_dir, 
		         string license_dir, bool isPC);
private:
	string _key;
	string _version;
	json get_variables(string func_file);
	string my_encryption(json j);
	string my_decryption(string str);
	void add_registration_time(json &j);
	void update(json &j, string decryp, string func_dir, 
		        string file, bool isPC);
	void set_version(string &res);
};
#endif