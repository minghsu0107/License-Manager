#ifndef GENERATE_UNIVERSAL_H
#define GENERATE_UNIVERSAL_H
#include <string>
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

class Universal {
public:
	Universal(string key, string version);
	string get_key();
	string get_version();
	void generate_universal_key();
private:
	string _key;
	string _version;
	string my_encryption(json j);
	void set_version(string &res);
};
#endif