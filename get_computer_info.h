#ifndef GET_COMPUTER_INFO_H
#define GET_COMPUTER_INFO_H
#include <string>
#include <vector>
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

class Extract {
public:
	Extract();
	Extract(string key);
	string get_key();
	json get_hardware_info();
	json get_projector_id(json j, string projector_file);
	string get_computer_name();
	string get_user_name();
	int load(string projector_file);
private:
	string _key;
	vector<string> execCmd(const char *cmd);
	vector<string> getHardDiskInfo();
	vector<string> getMacInfo();
	json add_os_info(json j);
	json add_registration_time(json j);
	string my_encryption(json j);
};
#endif
