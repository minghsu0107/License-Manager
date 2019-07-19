#ifndef GET_PROJECTOR_INFO_H
#define GET_PROJECTOR_INFO_H
#include <string>
#include <vector>
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

class ProExtract {
public:
	ProExtract();
	ProExtract(string key);
	string get_key();
	string getSerialNumber(string serial_number_file);
	json get_projector_info(string serial_number_file);
	int load(string serial_number_file, string output_dir_projector);
private:
	string _key;
	vector<string> getMacInfo();
	string my_encryption(json j);
};
#endif
