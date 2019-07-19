#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <direct.h>
#ifdef PC
#include "../pc_licensing/get_pc_info.h"
#include "../pc_licensing/license_validation_pc.h"
#endif
#ifdef PRO
#include "../projector_licensing/get_projector_info.h"
#include "../projector_licensing/license_validation_projector.h"
#endif
#include "../generator/license_generator.h"
#include "../generator/generate_universal.h"
#include "../json_convert/json.hpp"
using namespace std;
using json=nlohmann::json;

struct var {
	string key;
	string version;
	string output_dir_pc;
	string output_dir_projector;
	string serial_number_file;
	string email_dir_pc;
	string email_dir_projector;
	string service_dir_pc;
	string service_dir_projector;
	string license_dir;
	string output_file_pc;
	string output_file_projector;
};

var set_dir_path(string example_file) {
	ifstream t(example_file);
	json j;
	t >> j;
	j["email_dir_pc"] = string(j["output_dir_pc"]) + "/email_info_pc";
	j["email_dir_projector"] = string(j["output_dir_projector"])
	                           + "/email_info_projector";
	j["output_file_pc"] = string(j["output_dir_pc"])
	                      + "/final_result/result.json";
	j["output_file_projector"] = string(j["output_dir_projector"])
	                      + "/final_result/result.json";
	var v {
		j["key"].get<string>(),
		j["version"].get<string>(),
		j["output_dir_pc"].get<string>(), 
		j["output_dir_projector"].get<string>(),
		j["serial_number_file"].get<string>(),
		j["email_dir_pc"].get<string>(),
		j["email_dir_projector"].get<string>(), 
		j["service_dir_pc"].get<string>(),
		j["service_dir_projector"].get<string>(),  
		j["license_dir"].get<string>(),
		j["output_file_pc"].get<string>(),
		j["output_file_projector"].get<string>() 
	};
	t.close();
	return v;
}

void display(string output_file) {
	ifstream t(output_file);
	stringstream buffer;
	buffer << t.rdbuf();
	json j = json::parse(string(buffer.str()));

	cout.fill('.');
	j = j["service_status"];
	for (auto it = j.begin(); it != j.end(); ++it) {
		cout << left << setw(25) << it.key();
		if (it.value()) cout << "on" << endl;
		else cout << "off" << endl;
	}
}

bool generate_test(var v, bool isPC) {
	Generator g(v.key, v.version);
	int gmes;
	if (isPC) {
		gmes = g.generate(v.email_dir_pc, v.service_dir_pc, 
		                  v.license_dir, isPC);
	}
	else {
		gmes = g.generate(v.email_dir_projector, v.service_dir_projector, 
		                  v.license_dir, isPC); 
	}
	if (!gmes) return true;
	else if (gmes == 1011)
		cout << "fail to open email information file." << endl;
	else if (gmes == 1012)
		cout << "email information is damaged." << endl;
	else if (gmes == 1013)
		cout << "pc status file does not exist." << endl;
	else if (gmes == 1014)
		cout << "projector status file does not exist." << endl;
	return false;
}

#ifdef PC
bool extract_test_pc(var v) {
	Extract e(v.key);
	int emes = e.load(v.output_dir_pc);
	if (!emes)
		return true;
	else if (emes == 101) {
		cout << "fail to access pc information." << endl;
	}
	return false;
}
void validate_test_pc(var v) {
	Validation vv(v.key);
	int vmes = vv.validate(v.license_dir, v.output_dir_pc);

	if (!vmes || vmes == 10) {
		cout << "validation succeed, welcome!" << endl;
		if (vmes == 10)
			cout << "using universal key..." << endl;
		else if (!vmes) {
			cout << endl << "activate following functions..." << endl;
			display(v.output_file_pc);
		}
	}
	else if (vmes == 101)
		cout << "fail to access pc information." <<endl;
	else if (vmes == 1001)
		cout << "wrong license key." << endl;
	else if (vmes == 1002)
		cout << "license expired." << endl;
	else if (vmes == 1003)
		cout << "invalid pc." << endl;
}
void universal_test_pc(var v) {
	Universal u(v.key, v.version);
	u.generate_universal_key(v.license_dir, true);
	validate_test_pc(v);
}
void test_pc(var v) {
	if (extract_test_pc(v)) {
		if (generate_test(v, true)) {
			validate_test_pc(v);
		}
	}
}
#endif
#ifdef PRO
bool extract_test_projector(var v) {
	ProExtract e(v.key);
	int emes = e.load(v.serial_number_file, v.output_dir_projector);
	if (!emes)
		return true;
	else if (emes == 102) {
		cout << "fail to access projector information." << endl;
	}
	return false;
}
void validate_test_projector(var v) {
	ProValidation vv(v.key);
	int vmes = vv.validate(v.license_dir, v.output_dir_projector,
		                   v.serial_number_file);

	if (!vmes || vmes == 11) {
		cout << "validation succeed, welcome!" << endl;
		if (vmes == 11)
			cout << "using universal key..." << endl;
		else if (!vmes) {
			cout << endl << "activate following functions..." << endl;
			display(v.output_file_projector);
		}
	}
	else if (vmes == 102)
		cout << "fail to access projector information." <<endl;
	else if (vmes == 1004)
		cout << "wrong license key." << endl;
	else if (vmes == 1005)
		cout << "invalid pc." << endl;
}
void universal_test_projector(var v) {
	Universal u(v.key, v.version);
	u.generate_universal_key(v.license_dir, false);
	validate_test_projector(v);
}
void test_projector(var v) {
	if (extract_test_projector(v)) {
		if (generate_test(v, false)) {
			validate_test_projector(v);
		}
	}
}
#endif

int main(int argc, const char *argv[]) {
	var v = set_dir_path("src/example/example_input.json");
	
	const char *arg = argv[1];
#ifdef PC
	if (arg[0] == 'a')
		universal_test_pc(v);
	if (arg[0] == 'b')
		test_pc(v);
#endif
#ifdef PRO
	if (arg[0] == 'c')
		universal_test_projector(v);
	if (arg[0] == 'd')
		test_projector(v);
#endif
	return 0;
}
